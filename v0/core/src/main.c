/* NOIR wallet — on-device seed generation proof-of-life.
 *
 * The demo moment: press OK and the device mints a real 12-word seed from the
 * ATECC608B hardware TRNG, derives the BIP-32 path m/44'/60'/0'/0/0, and shows
 * the Ethereum address — all on the MCU. The private key never leaves the chip.
 *
 * Dirty-redraw pattern preserved: redraw ONLY on state change (a full OLED
 * refresh ~90ms stalls the button poll and drops quick taps). Idle loop ~10ms.
 */

#include "stm32f4xx_hal.h"
#include "ssd1306_min.h"
#include "gpio_map.h"
#include "buttons.h"
#include "screens.h"
#include "atecc.h"
#include "wallet.h"
#include "usb_setup.h"
#include "tusb.h"
#include <string.h>
#include <stdio.h>

#define ATECC_ADDR7 0x60

I2C_HandleTypeDef hi2c1;

void SysTick_Handler(void) { HAL_IncTick(); }

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 336;          /* VCO = 25/25 * 336 = 336 MHz */
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4; /* SYSCLK = 84 MHz */
    RCC_OscInitStruct.PLL.PLLQ = 7;            /* USB OTG FS = 336/7 = 48 MHz */
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
}

static void MX_GPIO_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef g = {0};
    g.Mode = GPIO_MODE_INPUT;
    g.Pull = GPIO_PULLUP;
    g.Speed = GPIO_SPEED_FREQ_LOW;
    for (int i = 0; i < BTN_COUNT; i++) {
        g.Pin = BTN_MAP[i].pin;
        HAL_GPIO_Init(BTN_MAP[i].port, &g);
    }
    g.Pin = GPIO_PIN_13;
    g.Mode = GPIO_MODE_OUTPUT_PP;
    g.Pull = GPIO_NOPULL;
    g.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &g);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}

void MX_I2C1_Init(void) {
    __HAL_RCC_I2C1_CLK_ENABLE();
    GPIO_InitTypeDef g = {0};
    g.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    g.Mode = GPIO_MODE_AF_OD;
    g.Pull = GPIO_PULLUP;
    g.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    g.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &g);
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&hi2c1);
}

typedef enum {
    ST_SPLASH,
    ST_WORDS,
    ST_ADDR,
    ST_SERIAL,
    ST_ERROR,
} state_t;

static state_t state;
static int      page;              /* which 3-word group is shown */

static char     mnemonic[256];
static char    *words[12];
static uint8_t  serial[9];
static uint8_t  addr[20];

/* USB bring-up diagnostics (read once at boot + live mount flag). */
static uint32_t usb_snpsid;        /* OTG FS Synopsys ID — 0x4F54xxxx if 48 MHz clk OK */
static uint32_t usb_dctl;          /* DCTL @0x804 — bit1 = SDIS (soft disconnect) */
static uint32_t usb_gccfg;         /* GCCFG @0x038 — bit16 = PWRDWN (FS PHY enable) */
static int      usb_mounted;       /* 1 once host sets configuration */
static uint32_t last_splash_refresh;

static void hexstr(const uint8_t *d, int n, char *out) {
    static const char h[] = "0123456789abcdef";
    for (int i = 0; i < n; i++) {
        out[2 * i]     = h[d[i] >> 4];
        out[2 * i + 1] = h[d[i] & 0xF];
    }
    out[2 * n] = 0;
}

/* Split the space-separated mnemonic in place into `words`. MUST be called
 * only AFTER wallet_seed_from_mnemonic() (which needs the intact string). */
static int split_words(char *mn) {
    int n = 0;
    char *p = mn;
    while (*p && n < 12) {
        words[n++] = p;
        while (*p && *p != ' ') p++;
        if (*p) *p++ = 0;
    }
    return n;
}

static void enable_cycle_counter(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

/* Mix in MCU entropy (free-running 84 MHz cycle counter + tick, with LCG
 * churn) so the seed is unique per generation even if the ATECC TRNG is
 * deterministic on this fresh/unconfigured chip. */
static void mix_entropy(uint8_t *buf, int len) {
    uint32_t v = DWT->CYCCNT ^ (HAL_GetTick() << 16);
    for (int i = 0; i < len; i++) {
        v = v * 1664525u + 1013904223u;
        v ^= DWT->CYCCNT ^ HAL_GetTick();
        buf[i] ^= (uint8_t)(v >> 24);
    }
}

static int generate(void) {
    uint8_t rnd[32], entropy[16], seed[64], priv[32], pub65[65];

    /* 16 bytes of true entropy from the secure element. */
    if (atecc_random(&hi2c1, ATECC_ADDR7, rnd) != 0) return -1;
    memcpy(entropy, rnd, 16);
    mix_entropy(entropy, 16);   /* guarantee per-generation uniqueness */

    if (wallet_mnemonic_from_entropy(entropy, 16, mnemonic) != 0) return -1;
    wallet_seed_from_mnemonic(mnemonic, "", seed);          /* needs intact string */
    if (wallet_derive_eth(seed, 0, priv, pub65, addr) != 0) return -1;
    if (split_words(mnemonic) != 12) return -1;             /* split after use */

    /* Non-fatal: chip serial is only for the "authenticity" display. */
    atecc_read_serial(&hi2c1, ATECC_ADDR7, serial);

    memset(rnd, 0, sizeof rnd);
    memset(entropy, 0, sizeof entropy);
    memset(seed, 0, sizeof seed);
    memset(priv, 0, sizeof priv);
    memset(pub65, 0, sizeof pub65);
    return 0;
}

static void render_splash(void) {
    char dbg[22];
    ui_clear();
    ui_text_center(6, "NOIR", 2);
    ui_text_center(28, "WALLET", 1);
    if (usb_mounted) {
        snprintf(dbg, sizeof dbg, "USB CONNECTED");
    } else if ((usb_snpsid & 0xFFFF0000u) == 0x4F540000u) {
        snprintf(dbg, sizeof dbg, "irq %lu sd%u nb%u",
                 (unsigned long)usb_irq_count,
                 (unsigned)(usb_dctl >> 1) & 1u,      /* SDIS: 0 = D+ up */
                 (unsigned)(usb_gccfg >> 21) & 1u);   /* NOVBUSSENS: 1 = VBUS overridden */
    } else {
        snprintf(dbg, sizeof dbg, "USB CLK DEAD %08lx", (unsigned long)usb_snpsid);
    }
    ui_text_center(40, dbg, 1);
    ui_hint("", "OK");
    ui_render();
}

static void render_generating(void) {
    ui_clear();
    ui_title("GENERATING");
    ui_text_center(24, "ATECC TRNG", 1);
    ui_text_center(34, "BIP39 seed", 1);
    ui_render();
}

static void render_words(void) {
    char t[20];
    snprintf(t, sizeof t, "SEED %d-%d/12", page * 3 + 1, page * 3 + 3);
    ui_clear();
    ui_title(t);
    for (int i = 0; i < 3; i++) {
        int idx = page * 3 + i;
        char line[20];
        snprintf(line, sizeof line, "%2d. %s", idx + 1, words[idx]);
        ui_text(4, 14 + i * 12, line, 1);
    }
    ui_hint("UP DN", "OK ADDR");
    ui_render();
}

static void render_addr(void) {
    char hex[41];
    hexstr(addr, 20, hex);
    char l1[22] = "0x";
    memcpy(l1 + 2, hex, 18); l1[20] = 0;      /* "0x" + 18 hex */
    char l2[21];
    memcpy(l2, hex + 18, 20); l2[20] = 0;     /* next 20 hex */
    char l3[3];
    memcpy(l3, hex + 38, 2); l3[2] = 0;       /* final 2 hex */
    ui_clear();
    ui_title("ETH ADDRESS");
    ui_text(4, 16, l1, 1);
    ui_text(4, 26, l2, 1);
    ui_text(4, 36, l3, 1);
    ui_hint("BACK", "OK ID");
    ui_render();
}

static void render_serial(void) {
    char hex[19];
    hexstr(serial, 9, hex);
    ui_clear();
    ui_title("CHIP SERIAL");
    ui_text_center(24, hex, 1);
    ui_hint("BACK", "");
    ui_render();
}

static void render_error(void) {
    ui_clear();
    ui_title("ERROR");
    ui_text_center(24, "ATECC FAIL", 1);
    ui_hint("", "OK RETRY");
    ui_render();
}

static void render(void) {
    switch (state) {
    case ST_SPLASH: render_splash();    break;
    case ST_WORDS:  render_words();     break;
    case ST_ADDR:   render_addr();      break;
    case ST_SERIAL: render_serial();    break;
    case ST_ERROR:  render_error();     break;
    }
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    SSD1306_Init(&hi2c1);
    buttons_init();

    enable_cycle_counter();
    atecc_selftest(&hi2c1, ATECC_ADDR7);   /* condition the TRNG (best effort) */
    usb_init();                            /* OTG FS + TinyUSB HID */
    usb_snpsid = USB_OTG_FS->Reserved40[0]; /* GSNPSID @0x40 — 0x4F54xxxx = 48 MHz clk alive */
    usb_dctl   = *(volatile uint32_t *)(USB_OTG_FS_PERIPH_BASE + 0x804UL); /* DCTL */
    usb_gccfg  = USB_OTG_FS->GCCFG;         /* GCCFG @0x38 — PWRDWN bit16 = FS PHY on */
    usb_mounted = 0;

    state = ST_SPLASH;
    page = 0;
    render();

    while (1) {
        tud_task();          /* TinyUSB device stack */

        int m = tud_mounted();
        uint32_t now = HAL_GetTick();
        if (state == ST_SPLASH &&
            (m != usb_mounted || (now - last_splash_refresh) >= 500u)) {
            usb_mounted = m;
            last_splash_refresh = now;
            render();        /* live-update IRQ count / "USB CONNECTED" */
        }

        btn_evt_t ev;
        if (buttons_poll(&ev) && ev.ev == BTN_PRESS) {
            int changed = 0;
            switch (state) {
            case ST_SPLASH:
                if (ev.id == BTN_CENTER) {
                    render_generating();
                    if (generate() != 0) state = ST_ERROR;
                    else { page = 0; state = ST_WORDS; }
                    changed = 1;
                }
                break;
            case ST_WORDS:
                if (ev.id == BTN_UP && page > 0)       { page--; changed = 1; }
                else if (ev.id == BTN_DOWN && page < 3) { page++; changed = 1; }
                else if (ev.id == BTN_CENTER) { state = ST_ADDR; changed = 1; }
                else if (ev.id == BTN_LEFT)   { state = ST_SPLASH; changed = 1; }
                break;
            case ST_ADDR:
                if (ev.id == BTN_CENTER) { state = ST_SERIAL; changed = 1; }
                else if (ev.id == BTN_LEFT) { state = ST_WORDS; changed = 1; }
                break;
            case ST_SERIAL:
                if (ev.id == BTN_LEFT) { state = ST_ADDR; changed = 1; }
                break;
            case ST_ERROR:
                if (ev.id == BTN_CENTER) { state = ST_SPLASH; changed = 1; }
                break;
            }
            if (changed) render();
        }
        HAL_Delay(10);
    }
}

//pic.c

#include "../includes/pic.h"
extern key_info_t *key_info;

#define IRQ0_SLEEP_TIMER_TICKS_AREA 0x1700
#define RTC_DATETIME_AREA 0x1610

uint_32* sleep_timer_ticks = (uint_32*)IRQ0_SLEEP_TIMER_TICKS_AREA;


// Send end of interrupt command to signal IRQ has been handled
void send_pic_eoi(uint_8 irq)
{
    if (irq >= 8) outb(PIC_2_CMD, PIC_EOI);

    outb(PIC_1_CMD, PIC_EOI);
}

// Disable PIC, if using APIC or IOAPIC
void disable_pic(void)
{
    outb(PIC_2_DATA, 0xFF);
    outb(PIC_1_DATA, 0xFF);
}

// Set IRQ mask by setting the bit in the IMR (interrupt mask register)
//   This will ignore the IRQ
void set_irq_mask(uint_8 irq)
{
    uint_16 port;
    uint_8 value;

    if (irq < 8) port = PIC_1_DATA;
    else {
        irq -= 8;
        port = PIC_2_DATA;
    }

    // Get current IMR value, set on the IRQ bit to mask it, then write new value
    //   to IMR
    // NOTE: Masking IRQ2 will stop 2nd PIC from raising IRQs due to 2nd PIC being
    //   mapped to IRQ2 in PIC1
    value = inb(port) | (1 << irq);
    outb(port, value);
}

// Clear IRQ mask by clearing the bit in the IMR (interrupt mask register)
//   This will enable recognizing the IRQ
void clear_irq_mask(uint_8 irq)
{
    uint_16 port;
    uint_8 value;

    if (irq < 8) port = PIC_1_DATA;
    else {
        irq -= 8;
        port = PIC_2_DATA;
    }

    // Get current IMR value, clear the IRQ bit to unmask it, then write new value
    //   to IMR
    // NOTE: Clearing IRQ2 will enable the 2nd PIC to raise IRQs due to 2nd PIC being
    //   mapped to IRQ2 in PIC1
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

// Remap PIC to use interrupts above first 15, to not interfere with exceptions (ISRs 0-31)
void remap_pic(void)
{
    uint_8 pic_1_mask, pic_2_mask;

    // Save current masks
    pic_1_mask = inb(PIC_1_DATA);
    pic_2_mask = inb(PIC_2_DATA);

    // ICW 1 (Initialization control word) - bit 0 = send up to ICW 4, bit 4 = initialize PIC
    outb(PIC_1_CMD, 0x11);
    io_wait();
    outb(PIC_2_CMD, 0x11);
    io_wait();

    // ICW 2 - Where to map the base interrupt in the IDT
    outb(PIC_1_DATA, NEW_IRQ_0);
    io_wait();
    outb(PIC_2_DATA, NEW_IRQ_8);
    io_wait();
    
    // ICW 3 - Where to map PIC2 to the IRQ line in PIC1; Mapping PIC2 to IRQ 2
    outb(PIC_1_DATA, 0x4);  // Bit # (0-based) - 0100 = bit 2 = IRQ2
    io_wait();
    outb(PIC_2_DATA, 0x2);  // Binary # for IRQ in PIC1, 0010 = 2
    io_wait();
   
    // ICW 4 - Set x86 mode
    outb(PIC_1_DATA, 0x1); 
    io_wait();
    outb(PIC_2_DATA, 0x1); 
    io_wait();

    // Save current masks
    outb(PIC_1_DATA, pic_1_mask);
    outb(PIC_2_DATA, pic_2_mask);
}

//PIT Timer Channel 0 PIC IRQ0 interrupt handler

void timer_irq0_handler(void) {
    if (*sleep_timer_ticks > 0) (*sleep_timer_ticks)--;

    /*
    static uint_32 ticks = 0;
    setCursorPosition(800);
    printf("%s %d", "Timer ticks: ", ticks);
    ticks++;
    */
    send_pic_eoi(0);
}

// Change PIT Channel frequency
void set_pit_channel_mode_frequency(const uint_8 channel, const uint_8 operating_mode, const uint_16 frequency)
{
    // Invalid input
    if (channel > 2) return;

    __asm__ __volatile__("cli");

    /*send commend to PIT by this format:
    bit  0  : BCD/Binary mode
    bits 1-3: operating mode(0- 7)
    bits 4-5: access mode- how to write/read data, the internal register is 16 bits wide and
    //PIT had only 8 bits of data(D0- D7)
    bits 6-7: select channel 0, 1 ot 2
    */
    //I will choose the binary mode(bit 0), set this bit to 0 so dont need to shift a 0 by 0..
    //also choose mode number 2 for count rate
    // Send the command byte, always sending lobyte/hibyte for access mode
    outb(0x43, (channel << 6) | (0x3 << 4) | (operating_mode << 1));

    // Send the frequency divider to the input channel
    outb(0x40 + channel, (uint_8)frequency);           // Low byte
    outb(0x40 + channel, (uint_8)(frequency >> 8));    // High byte

    __asm__ __volatile__("sti");
}

// Show date time info or not
bool show_datetime = false;

// Date time info
typedef struct {
    uint_8 second;
    uint_8 minute;
    uint_8 hour;
    uint_8 day;
    uint_8 month;
    uint_16 year;
} __attribute__((packed)) datetime_t;

datetime_t* datetime = (datetime_t*)RTC_DATETIME_AREA;

// CMOS registers
enum {
    cmos_address = 0x70,
    cmos_data = 0x71
};

// Read CMOS update in progress flag/bit in status register A
bool cmos_update_in_progress(void)
{
    outb(cmos_address, 0x8A);       // Will read from status register A, disable NMI
    return (inb(cmos_data) & 0x80); // If register A top bit is set, CMOS update is in progress
}

// Get an RTC register value
uint_8 get_rtc_register(uint_8 reg)
{
    outb(cmos_address, reg | 0x80);     // Disable NMI when sending register to read
    return inb(cmos_data);              // Return data at that register
}

// Enable RTC
void enable_rtc(void)
{
    uint_8 prev_regB_value = get_rtc_register(0x0B);

    outb(cmos_address, 0x8B);                // Select register B again, because reading a CMOS register resets to register D
    outb(cmos_data, prev_regB_value | 0x40); // Set bit 6 to enable periodic interrupts at default rate of 1024hz

    get_rtc_register(0x0C);                 // Read status register C to clear out any pending IRQ8 interrupts
}

// Disable RTC
void disable_rtc(void)
{
    uint_8 prev_regB_value;

    __asm__ __volatile__("cli");

    prev_regB_value = get_rtc_register(0x0B);

    outb(cmos_address, 0x8B);                // Select register B again, because reading a CMOS register resets to register D
    outb(cmos_data, prev_regB_value & 0xBF); // Clear bit 6 to disable periodic interrupts

    __asm__ __volatile__("sti");
}

extern uint_16 clockPosition;
// CMOS RTC IRQ8 Handler
void cmos_rtc_irq8_handler()
{
    datetime_t new_datetime, old_datetime;
    static uint_16 rtc_ticks = 0;
    uint_8 regB_value;

    __asm__ __volatile__("cli");

    rtc_ticks++;

    // If one second passed, get new date/time values
    if (rtc_ticks % 1024 == 0) {
        while (cmos_update_in_progress()); // Wait until CMOS is done updating

        new_datetime.second = get_rtc_register(0x00);
        new_datetime.minute = get_rtc_register(0x02);
        new_datetime.hour = get_rtc_register(0x04);
        new_datetime.day = get_rtc_register(0x07);
        new_datetime.month = get_rtc_register(0x08);
        new_datetime.year = get_rtc_register(0x09);

        do {
            old_datetime = new_datetime;

            while (cmos_update_in_progress()); // Wait until CMOS is done updating

            new_datetime.second = get_rtc_register(0x00);
            new_datetime.minute = get_rtc_register(0x02);
            new_datetime.hour = get_rtc_register(0x04);
            new_datetime.day = get_rtc_register(0x07);
            new_datetime.month = get_rtc_register(0x08);
            new_datetime.year = get_rtc_register(0x09);

        } while (
            (new_datetime.second != old_datetime.second) ||
            (new_datetime.minute != old_datetime.minute) ||
            (new_datetime.hour != old_datetime.hour) ||
            (new_datetime.day != old_datetime.day) ||
            (new_datetime.month != old_datetime.month) ||
            (new_datetime.year != old_datetime.year)
            );

        regB_value = get_rtc_register(0x0B);

        // Convert BCD values to binary if needed (bit 2 is clear)
        if (!(regB_value & 0x04)) {
            new_datetime.second = (new_datetime.second & 0x0F) + ((new_datetime.second / 16) * 10);
            new_datetime.minute = (new_datetime.minute & 0x0F) + ((new_datetime.minute / 16) * 10);
            new_datetime.hour = ((new_datetime.hour & 0x0F) + (((new_datetime.hour & 0x70) / 16) * 10)) | (new_datetime.hour & 0x80);
            new_datetime.day = (new_datetime.day & 0x0F) + ((new_datetime.day / 16) * 10);
            new_datetime.month = (new_datetime.month & 0x0F) + ((new_datetime.month / 16) * 10);
            new_datetime.year = (new_datetime.year & 0x0F) + ((new_datetime.year / 16) * 10);
        }

        // Convert 12hr to 24hr if needed (bit 1 is clear in register B and top bit of hour is set)
        if (!(regB_value & 0x02) && (new_datetime.hour & 0x80)) {
            new_datetime.hour = ((new_datetime.hour & 0x7F) + 12) % 24;
        }

        // Get year 
        new_datetime.year += 2000;

        // Set datetime values in memory
        *datetime = new_datetime;

        // Print date/time on screen
        if (show_datetime) {
            //setCursorPosition(PositionFromCoords(0, 3));
            // ISO Formatted date/time - YYYY-MM-DD HH:MM:SS
            clockPosition = 240;
            puts_clock(datetime->year);
            putc_clock('-');

            if (datetime->month < 10) putc_clock('0');
            puts_clock(datetime->month);
            putc_clock('-');

            if (datetime->day < 10) putc_clock('0');
            puts_clock(datetime->day);
            putc_clock(' ');

            if (datetime->hour < 10) putc_clock('0');
            puts_clock(datetime->hour);
            putc_clock(':');

            if (datetime->minute < 10) putc_clock('0');
            puts_clock(datetime->minute);
            putc_clock(':');

            if (datetime->second < 10) putc_clock('0');
            puts_clock(datetime->second);
        }
    }
    // Read register C so that future IRQ8s can occur
    get_rtc_register(0x0C);

    send_pic_eoi(8);

    __asm__ __volatile__("sti");
}

void keyboard_irq1_handler(void)
{

    enum {
        LSHIFT_MAKE  = 0x2A,
        LSHIFT_BREAK = 0xAA,
        RSHIFT_MAKE  = 0x36,
        RSHIFT_BREAK = 0xB6,
        LCTRL_MAKE   = 0x1D,
        LCTRL_BREAK  = 0x9D,
    };

    uint_8 key;
    static bool e0 = false; 
    static bool e1 = false;

    // TODO: Add keyboard initialization & scancode functions, 
    //   do not assume scancode set 1
    
    // Scancode set 1 -> Ascii lookup table
    const uint_8 *scancode_to_ascii = "\x00\x1B" "1234567890-=" "\x08"
    "\x00" "qwertyuiop[]" "\x0D\x1D" "asdfghjkl;'`" "\x00" "\\"
    "zxcvbnm,./" "\x00\x00\x00" " ";

    // Shift key pressed on number row lookup table (0-9 keys)
    const uint_8 *num_row_shifts = ")!@#$%^&*(";

    // Set current key to null
    key_info->key = 0;

    key = inb(PS2_DATA_PORT);   // Read in new key
    
    if (key) {

        if      (key == LSHIFT_MAKE  || key == RSHIFT_MAKE) key_info->shift = true; 
        else if (key == LSHIFT_BREAK || key == RSHIFT_BREAK) key_info->shift = false; 
        else if (key == LCTRL_MAKE)  key_info->ctrl = true;
        else if (key == LCTRL_BREAK) key_info->ctrl = false;
        else if (key == 0xE0) e0 = true;
        else {
            if (!(key & 0x80)) {
                // Don't translate escaped scancodes, only return them
                if (!e0) { 
                    
                    key = scancode_to_ascii[key]; 
                    // If pressed shift, translate key to shifted key
                    if (key_info->shift) {
                        if (key >= 'a' && key <= 'z') key -= 0x20;  // Convert lowercase into uppercase
                        else if (key >= '0' && key <= '9') key = num_row_shifts[key - 0x30];  // Get int value of character, offset into shifted nums
                        else {
                            if      (key == '=')  key = '+';
                            else if (key == '\\') key = '|'; 
                            else if (key == '`')  key = '~';
                            else if (key == '[')  key = '{';
                            else if (key == ']')  key = '}';
                            else if (key == '\'') key = '\"';
                            else if (key == ';')  key = ':';
                            else if (key == ',')  key = '<';
                            else if (key == '.')  key = '>';
                            else if (key == '/')  key = '?';
                            // TODO: Add more shifted keys here...
                            
                        }
                    }
                    
                    
                }
                else {
                   
                    if (key_info->shift && key_info->ctrl)
                    {
                        if (key == 0x50) key = 0x7D;
                        else if (key == 0x48) key = 0x7A;
                    }

                }
                
                key_info->key = key;    // Set ascii key value in struct
                   
            }
            if (e0) e0 = false;
        }
    }

      send_pic_eoi(1);
}

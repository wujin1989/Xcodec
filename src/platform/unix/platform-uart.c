/** Copyright (c) 2025, Wu Jin <wujin.developer@gmail.com>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

#include "platform/platform-uart.h"

void platform_uart_close(platform_uart_t uart) {
    close(uart);
}

int platform_uart_read(platform_uart_t uart, uint8_t* buf, int len) {
    ssize_t off = 0;
    while (off < len) {
        ssize_t tmp;
        do {
            tmp = read(uart, buf + off, len - (int)off);
        } while (tmp == PLATFORM_UA_ERROR_UART_ERROR && errno == EINTR);
        if (tmp == PLATFORM_UA_ERROR_UART_ERROR) {
            return PLATFORM_UA_ERROR_UART_ERROR;
        }
        if (tmp == 0) {
            return off;
        }
        off += tmp;
    }
    return (int)off;
}

int platform_uart_write(platform_uart_t uart, uint8_t* buf, int len) {
    ssize_t off = 0;
    while (off < len) {
        ssize_t tmp;
        do {
            tmp = write(uart, buf + off, len - (int)off);
        } while (tmp == PLATFORM_UA_ERROR_UART_ERROR && errno == EINTR);
        if (tmp == PLATFORM_UA_ERROR_UART_ERROR) {
            return PLATFORM_UA_ERROR_UART_ERROR;
        }
        off += tmp;
    }
    return (int)off;
}

platform_uart_t platform_uart_open(platform_uart_config_t* config) {
    platform_uart_t uart = open(config->device, O_RDWR | O_NOCTTY);
    if (uart == -1) {
        return PLATFORM_UA_ERROR_INVALID_UART;
    }
    struct termios options;
    if (tcgetattr(uart, &options) != 0) {
        platform_uart_close(uart);
        return PLATFORM_UA_ERROR_INVALID_UART;
    }
    memset(&options, 0, sizeof(struct termios));
    speed_t baudrate_flag;
    switch (config->baudrate) {
    case PLATFORM_UART_BAUDRATE_9600:
        baudrate_flag = B9600;
        break;
    case PLATFORM_UART_BAUDRATE_19200:
        baudrate_flag = B19200;
        break;
    case PLATFORM_UART_BAUDRATE_38400:
        baudrate_flag = B38400;
        break;
    case PLATFORM_UART_BAUDRATE_57600:
        baudrate_flag = B57600;
        break;
    case PLATFORM_UART_BAUDRATE_115200:
        baudrate_flag = B115200;
        break;
    default:
        platform_uart_close(uart);
        return PLATFORM_UA_ERROR_INVALID_UART;
    }
    cfsetispeed(&options, baudrate_flag);
    cfsetospeed(&options, baudrate_flag);

    options.c_cflag &= ~CSIZE;
    switch (config->databits) {
    case PLATFORM_UART_DATABITS_CS7:
        options.c_cflag |= CS7;
        break;
    case PLATFORM_UART_DATABITS_CS8:
        options.c_cflag |= CS8;
        break;
    default:
        platform_uart_close(uart);
        return PLATFORM_UA_ERROR_INVALID_UART;
    }
    switch (config->stopbits) {
    case PLATFORM_UART_STOPBITS_ONE:
        options.c_cflag &= ~CSTOPB;
        break;
    case PLATFORM_UART_STOPBITS_TWO:
        options.c_cflag |= CSTOPB;
        break;
    default:
        platform_uart_close(uart);
        return PLATFORM_UA_ERROR_INVALID_UART;
    }
    switch (config->parity) {
    case PLATFORM_UART_PARITY_NO:
        options.c_cflag &= ~PARENB;
        break;
    case PLATFORM_UART_PARITY_ODD:
        options.c_cflag |= PARENB;
        options.c_cflag |= PARODD;
        break;
    case PLATFORM_UART_PARITY_EVEN:
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
        break;
    default:
        platform_uart_close(uart);
        return PLATFORM_UA_ERROR_INVALID_UART;
    }
    options.c_cflag |= CLOCAL | CREAD;

    if (config->timeout_ms) {
        options.c_cc[VMIN] = 0;
        options.c_cc[VTIME] = (config->timeout_ms / 1000UL) * 10;
    } else {
        options.c_cc[VMIN] = 1;
        options.c_cc[VTIME] = 0;
    }
    tcflush(uart, TCIOFLUSH);
    if (tcsetattr(uart, TCSANOW, &options) != 0) {
        platform_uart_close(uart);
        return PLATFORM_UA_ERROR_INVALID_UART;
    }
    return uart;
}
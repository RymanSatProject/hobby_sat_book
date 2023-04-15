#include "terminal.hpp"
#include "parsers/cmdparse.hpp"

constexpr uint16_t TERMINAL_BUFLEN = 256;
constexpr size_t   ARG_MAXNUM = 10;

static uint16_t    current_id = 0xFFFF;
static Telecommand cmd(COMM_BUF_SIZE);

Terminal::Terminal(Serial *serial, MainSystem *sys) {
    this->serial = serial;
    input = new Array<uint8_t>(TERMINAL_BUFLEN + 1);
    memset(input->buf, 0, input->size);
    input->size = 0;
    full = false;

    arg = new Array<char *>(ARG_MAXNUM);

    this->sys = sys;

    print_head();
}

void Terminal::run(void) {
    get_input();

    if (!full) {
        return;
    }

    int ret = find_command(cmd);
    clear_input();

    if (ret) {
        return;
    }

    if (set_payload(cmd, *arg)) {
        print_head();
        return;
    }

    sys->cmd->exec(cmd);
    cmd.cmd = 0;
    print_head();
}

void Terminal::get_input(void) {
    if (!serial->readable()) {
        return;
    }

    while (serial->readable()) {
        char c = serial->getc();

        if (c == '\n' || c == '\r' || input->size >= TERMINAL_BUFLEN - 1) {
            serial->puts("\r\n");
            input->buf[input->size++] = '\0';
            full = true;
            break;

        } else if (c == '\b') {
            // Backspace
            if (input->size > 0) {
                serial->puts("\b \b");
                input->size--;
            }

        } else if (c == 0x3) {
            // Ctrl + C
            serial->puts("\r\n");
            print_head();
            clear_input();

        } else if (c <= 0x1f) {
            // Ignore other non-character input
            continue;

        } else {
            serial->putc(c);
            input->buf[input->size++] = c;
        }
    }
}

void Terminal::clear_input(void) {
    full = false;
    input->size = 0;
}

static void nullify_space(Array<uint8_t> &arr) {
    for (size_t i = 0; i < arr.size; i++) {
        if (arr[i] == ' ') {
            arr[i] = '\0';
        }
    }
}

static void parse_args(Array<uint8_t> &input, Array<char *> &arg,
                       size_t maxnum) {
    if (input.buf[0] == '\0') {
        arg.size = 0;
        return;
    }

    for (size_t i = 1; i < ARG_MAXNUM; i++) {
        arg.buf[i] = nullptr;
    }

    arg.buf[0] = (char *)input.buf;
    size_t num = 1;
    bool   zeroed = false;

    for (size_t i = 1; i < input.size && num < maxnum; i++) {
        if (input.buf[i] == '\0') {
            zeroed = true;

        } else {
            if (zeroed) {
                arg.buf[num] = (char *)&input.buf[i];
                num++;
            }

            zeroed = false;
        }
    }

    arg.size = num;
}

int Terminal::find_command(Telecommand &cmd) {
    nullify_space(*input);
    parse_args(*input, *arg, ARG_MAXNUM);

    if (arg->size == 0) {
        print_head();
        return 1;
    }

    bool found = false;

    for (size_t i = 0; i < ARRAY_SIZE(parserlist); i++) {
        if (strcmp(arg->buf[0], parserlist[i].name) == 0) {
            cmd.cmd = parserlist[i].cmd;
            cmd.id = current_id--;
            found = true;
        }
    }

    if (!found) {
        serial->printf("Terminal: command not found: %s\n", input->buf);
    }

    return 0;
}

int Terminal::set_payload(Telecommand &cmd, Array<char *> &arg) {
    for (size_t i = 0; i < ARRAY_SIZE(parserlist); i++) {
        if (cmd.cmd == parserlist[i].cmd) {
            if (strcmp("-h", arg[1]) == 0) {
                logger_base(parserlist[i].help);
                logger_base("\n");
                return 1;
            }

            return parserlist[i].parse(cmd, arg);
        }
    }

    return 1;
}

void Terminal::print_head(void) {
    serial->puts("obc $ ");
}

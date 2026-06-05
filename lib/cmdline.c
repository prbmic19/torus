/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <torus/types.h>
#include <lib/cmdline.h>
#include <lib/string.h>

static int _isspace(int c)
{
    return c <= ' ';
}

/*
 * Find a Boolean option in the command line.
 *
 * Returns 0 if the option was not found or the
 * position of that option (starting from 1) if it was found.
 */
int cmdline_find_option_bool(const char *cmdline, const char *option)
{
    if (!cmdline || !option)
    {
        return -1;
    }

    if (!*cmdline || !*option)
    {
        return 0;
    }

    int pos = 1;
    size_t option_len = strlen(option);

    while (*cmdline)
    {
        const char *word;
        size_t word_len = 0;
        bool is_key_value = false;

        while (_isspace(*cmdline))
        {
            cmdline++;
        }

        if (!*cmdline)
        {
            break;
        }

        word = cmdline;

        while (*cmdline && !_isspace(*cmdline))
        {
            if (*cmdline == '=')
            {
                is_key_value = true;
            }

            cmdline++;
            word_len++;
        }

        if (!is_key_value && word_len == option_len && strncmp(word, option, word_len) == 0)
        {
            return pos;
        }

        pos++;
    }

    return 0;
}

/*
* Copyright (c) 2018 naehrwert
* Copyright (c) 2019-2024 CTCaer
*
* This program is free software; you can redistribute it and/or modify it
* under the terms and conditions of the GNU General Public License,
* version 2, as published by the Free Software Foundation.
*
* This program is distributed in the hope it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdarg.h>
#include <string.h>

#include <utils/types.h>

char **sout_buf;

static void _s_putc(char c)
{
	**sout_buf = c;
	*sout_buf += 1;
}

static void _s_puts(char *s)
{
	for (; *s; s++)
		_s_putc(*s);
}

static void _s_putn(u32 v, int base, char fill, int fcnt)
{
	static const char digits[] = "0123456789ABCDEF";

	char *p;
	char buf[65];
	int c = fcnt;
	bool negative = false;

	if (base != 10 && base != 16)
		return;

	// Account for negative numbers.
	if (base == 10 && v & 0x80000000)
	{
		negative = true;
		v = (int)v * -1;
		c--;
	}

	p = buf + 64;
	*p = 0;
	do
	{
		c--;
		*--p = digits[v % base];
		v /= base;
	} while (v);

	if (negative)
		*--p = '-';

	if (fill != 0)
	{
		while (c > 0 && p > buf)
		{
			*--p = fill;
			c--;
		}
	}

	_s_puts(p);
}

void s_printf(char *out_buf, const char *fmt, ...)
{
	va_list ap;
	int fill, fcnt;

	sout_buf = &out_buf;

	va_start(ap, fmt);
	while (*fmt)
	{
		if (*fmt == '%')
		{
			fmt++;
			fill = 0;
			fcnt = 0;
			if ((*fmt >= '0' && *fmt <= '9') || *fmt == ' ')
			{
				fcnt = *fmt;
				fmt++;
				if (*fmt >= '0' && *fmt <= '9')
				{
					fill = fcnt;
					fcnt = *fmt - '0';
					fmt++;
				}
				else
				{
					fill = ' ';
					fcnt -= '0';
				}
			}
			switch (*fmt)
			{
			case 'c':
				char c = va_arg(ap, u32);
				if (c != '\0')
					_s_putc(c);
				break;
			case 's':
				_s_puts(va_arg(ap, char *));
				break;
			case 'd':
				_s_putn(va_arg(ap, u32), 10, fill, fcnt);
				break;
			case 'p':
			case 'P':
			case 'x':
			case 'X':
				_s_putn(va_arg(ap, u32), 16, fill, fcnt);
				break;
			case '%':
				_s_putc('%');
				break;
			case '\0':
				goto out;
			default:
				_s_putc('%');
				_s_putc(*fmt);
				break;
			}
		}
		else
			_s_putc(*fmt);
		fmt++;
	}

out:
	**sout_buf = '\0';
	va_end(ap);
}

void s_vprintf(char *out_buf, const char *fmt, va_list ap)
{
	int fill, fcnt;

	sout_buf = &out_buf;

	while (*fmt)
	{
		if (*fmt == '%')
		{
			fmt++;
			fill = 0;
			fcnt = 0;
			if ((*fmt >= '0' && *fmt <= '9') || *fmt == ' ')
			{
				fcnt = *fmt;
				fmt++;
				if (*fmt >= '0' && *fmt <= '9')
				{
					fill = fcnt;
					fcnt = *fmt - '0';
					fmt++;
				}
				else
				{
					fill = ' ';
					fcnt -= '0';
				}
			}
			switch(*fmt)
			{
			case 'c':
				_s_putc(va_arg(ap, u32));
				break;
			case 's':
				_s_puts(va_arg(ap, char *));
				break;
			case 'd':
				_s_putn(va_arg(ap, u32), 10, fill, fcnt);
				break;
			case 'p':
			case 'P':
			case 'x':
			case 'X':
				_s_putn(va_arg(ap, u32), 16, fill, fcnt);
				break;
			case '%':
				_s_putc('%');
				break;
			case '\0':
				goto out;
			default:
				_s_putc('%');
				_s_putc(*fmt);
				break;
			}
		}
		else
			_s_putc(*fmt);
		fmt++;
	}

out:
	**sout_buf = '\0';
}

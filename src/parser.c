#line 1 "src/parser.rl"
/*
 * Copyright (c) 2008-2009 Ricardo Martins <ricardo@scarybox.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <string.h>

#include "command.h"
#include "common.h"
#include "helper.h"
#include "queue.h"

struct params
{
    char buffer[BUFSIZE+1];
    char stdin[BUFSIZE+1];
    char stdout[BUFSIZE+1];
    int buflen;
    int stdin_len;
    int stdout_len;
    int cs;
};

#line 116 "src/parser.rl"



#line 47 "src/parser.c"
static const char _parser_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6, 1, 
	7, 2, 3, 0, 2, 4, 3, 3, 
	4, 3, 0
};

static const char _parser_key_offsets[] = {
	0, 0, 9, 15, 21, 30, 39, 48, 
	54, 63, 69, 78, 84, 93, 99, 108
};

static const char _parser_trans_keys[] = {
	0, 32, 38, 62, 124, 9, 13, 59, 
	60, 0, 38, 59, 60, 62, 124, 0, 
	32, 59, 124, 9, 13, 0, 32, 38, 
	62, 124, 9, 13, 59, 60, 0, 32, 
	38, 62, 124, 9, 13, 59, 60, 0, 
	32, 38, 62, 124, 9, 13, 59, 60, 
	0, 38, 59, 60, 62, 124, 0, 32, 
	38, 62, 124, 9, 13, 59, 60, 0, 
	38, 59, 60, 62, 124, 0, 32, 38, 
	62, 124, 9, 13, 59, 60, 0, 38, 
	59, 60, 62, 124, 0, 32, 38, 62, 
	124, 9, 13, 59, 60, 0, 38, 59, 
	60, 62, 124, 0, 32, 38, 62, 124, 
	9, 13, 59, 60, 0
};

static const char _parser_single_lengths[] = {
	0, 5, 6, 4, 5, 5, 5, 6, 
	5, 6, 5, 6, 5, 6, 5, 0
};

static const char _parser_range_lengths[] = {
	0, 2, 0, 1, 2, 2, 2, 0, 
	2, 0, 2, 0, 2, 0, 2, 0
};

static const char _parser_index_offsets[] = {
	0, 0, 8, 15, 21, 29, 37, 45, 
	52, 60, 67, 75, 82, 90, 97, 105
};

static const char _parser_indicies[] = {
	1, 2, 3, 3, 3, 2, 3, 0, 
	1, 5, 6, 7, 8, 9, 4, 1, 
	10, 6, 9, 10, 3, 3, 12, 3, 
	3, 3, 12, 3, 11, 3, 13, 3, 
	3, 3, 13, 3, 4, 3, 7, 3, 
	3, 3, 7, 3, 14, 1, 5, 6, 
	3, 15, 9, 14, 3, 15, 3, 3, 
	3, 15, 3, 16, 1, 5, 6, 3, 
	3, 9, 16, 3, 8, 3, 3, 3, 
	8, 3, 17, 1, 5, 6, 18, 3, 
	9, 17, 3, 18, 3, 3, 3, 18, 
	3, 19, 1, 5, 6, 3, 3, 9, 
	19, 1, 20, 3, 3, 3, 20, 3, 
	4, 3, 0
};

static const char _parser_trans_targs[] = {
	2, 15, 14, 0, 2, 3, 4, 6, 
	10, 4, 3, 2, 5, 5, 7, 8, 
	9, 11, 12, 13, 14
};

static const char _parser_trans_actions[] = {
	17, 9, 7, 0, 1, 13, 15, 0, 
	0, 11, 0, 23, 20, 0, 3, 0, 
	5, 5, 0, 3, 0
};

static const int parser_start = 1;
static const int parser_first_final = 15;
static const int parser_error = 0;

static const int parser_en_main = 1;

#line 119 "src/parser.rl"

queue_t *parseInput(char *buffer)
{
    const char *p  = buffer;
    const char *pe = buffer + strlen(buffer)+1;
    struct params *fsm = malloc(sizeof(struct params));
    queue_t *queue = queueNew();
    command_t *command = NULL;

    fsm->buflen = 0;

    
#line 140 "src/parser.c"
	{
	 fsm->cs = parser_start;
	}
#line 131 "src/parser.rl"
    
#line 146 "src/parser.c"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if (  fsm->cs == 0 )
		goto _out;
_resume:
	_keys = _parser_trans_keys + _parser_key_offsets[ fsm->cs];
	_trans = _parser_index_offsets[ fsm->cs];

	_klen = _parser_single_lengths[ fsm->cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _parser_range_lengths[ fsm->cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += ((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _parser_indicies[_trans];
	 fsm->cs = _parser_trans_targs[_trans];

	if ( _parser_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _parser_actions + _parser_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 45 "src/parser.rl"
	{
        if (fsm->buflen < BUFSIZE) {
            fsm->buffer[fsm->buflen++] = (*p);
            fsm->buffer[fsm->buflen]   = 0;
        }
    }
	break;
	case 1:
#line 52 "src/parser.rl"
	{
        if (fsm->stdin_len < BUFSIZE) {
            fsm->stdin[fsm->stdin_len++] = (*p);
            fsm->stdin[fsm->stdin_len]   = 0;
        }
    }
	break;
	case 2:
#line 59 "src/parser.rl"
	{
        if (fsm->stdout_len < BUFSIZE) {
            fsm->stdout[fsm->stdout_len++] = (*p);
            fsm->stdout[fsm->stdout_len]   = 0;
        }
    }
	break;
	case 3:
#line 66 "src/parser.rl"
	{
        fsm->buflen = 0;
        fsm->stdin_len = 0;
        fsm->stdout_len = 0;
        fsm->buffer[0] = 0;
        fsm->stdout[0] = 0;
        fsm->stdin[0] = 0;
        command = commandNew();
    }
	break;
	case 4:
#line 76 "src/parser.rl"
	{
        if (fsm->buflen > 0) {
            char *tmp = expandAlias(fsm->buffer);
            if (expandGlob(command, tmp) != 0) {
                fprintf(stderr, "Illegal character found, aborting. Blame wordexp.\n");
                free(tmp);
                {p++; goto _out; }
            }
            free(tmp);
            command->path = command->argv[0];
            if (fsm->stdin_len > 0)
                command->redirectFromPath = expand(fsm->stdin);
            if (fsm->stdout_len > 0)
                command->redirectToPath = expand(fsm->stdout);
            queueInsert(queue, command, (queueNodeFreeFunction) commandFree);
            command = NULL;
        }
    }
	break;
	case 5:
#line 96 "src/parser.rl"
	{ command->connectionMask = commandConnectionPipe; }
	break;
	case 6:
#line 97 "src/parser.rl"
	{ command->connectionMask = commandConnectionBackground; }
	break;
	case 7:
#line 98 "src/parser.rl"
	{
        if (command->connectionMask != commandConnectionBackground)
            command->connectionMask = commandConnectionSequential;
    }
	break;
#line 295 "src/parser.c"
		}
	}

_again:
	if (  fsm->cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}
#line 132 "src/parser.rl"

    if (fsm->cs == parser_error) {
        fprintf(stderr, "Parser error near `%c%c'\n", *(p-1), *p);
        queueFree(queue);
        queue = queueNew();
    }

    if (command != NULL)
        free(command);
    free(fsm);
    return queue;
}

// vim: et ts=4 sw=4 sts=4

import os

def CheckRagel(context):
    context.Message("Checking for ragel... ")
    result = os.popen("which ragel 2>/dev/null").read() != ''
    context.Result(result)
    return result

"""
Set up options.
"""
opts = Variables()
opts.AddVariables(
  ('prefix', 'install architecture-independent files in', '/usr/local'),
  ('bindir', 'user executables', 'PREFIX/bin'),
  ('mandir', 'man documentation', 'PREFIX/man'),
)

opts.Add(BoolVariable("libedit", "Set to link to libedit instead of readline", "no"))

"""
Set up environment.
"""
env = Environment(options = opts)
Help(opts.GenerateHelpText(env))
env['CFLAGS'] = Split('-Os -Wall')

conf = Configure(env, custom_tests = {'CheckRagel' : CheckRagel})

if not env['libedit']:
    readline = conf.CheckLib('readline')
    if not readline and not conf.CheckLib('libedit'):
        print 'Did not find either readline or libedit, exiting'
        Exit(1)
elif not conf.CheckLib('libedit'):
    print 'Did not find libedit, exiting'
    Exit(1)

if env['libedit']:
    env.ParseConfig("pkg-config --cflags --libs libedit")
else:
    env.Append(CPPDEFINES='GNU_READLINE')

if conf.CheckRagel():
    env['BUILDERS']['Ragel'] = Builder(action="ragel -C $SOURCE -o $TARGET")
    env.Ragel("src/parser.c", ["src/parser.rl"])

debug = int(ARGUMENTS.get('debug', '0'))
if debug:
    env.Append(CPPDEFINES=['DEBUG'])
    env.Append(CFLAGS=['-g'])
else:
    env.Append(LINKFLAGS=['-s'])

testing = env.Clone()
testing.Append(CPPPATH = ['build/cmockery/include/google', 'src'])

"""
Set up install path.
"""
prefix  = env['prefix']
bin_dir = env['bindir'].replace('PREFIX', prefix)
man_dir = env['mandir'].replace('PREFIX', prefix)

"""
Compile the program.
"""
sources = Glob('src/*.c')
tests = Glob('tests/*.c')
objs = ['src/alias.o', 'src/helper.o', 'src/builtins.o', 'src/queue.o',
        'src/command.o', 'src/parser.o', 'src/hash-table.o', 'src/hash-string.o',
        'src/compare-string.o']

redshell = env.Program('redshell', sources)
runtests = testing.Program('run_tests', tests + objs +
        ['build/cmockery/lib/libcmockery.a'])
Depends(runtests, redshell)
Default(redshell)

"""
Install the program.
"""
env.Install(bin_dir, redshell)

env.Alias('install', prefix)
env.Alias('tests', runtests)

# vim: filetype=python

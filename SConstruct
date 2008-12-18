"""
Set up options.
"""
opts = Options()
opts.AddOptions(
  ('prefix', 'install architecture-independent files in', '/usr/local'),
  ('bindir', 'user executables', 'PREFIX/bin'),
  ('mandir', 'man documentation', 'PREFIX/man'),
)

"""
Set up environment.
"""
env = Environment(options = opts)
Help(opts.GenerateHelpText(env))
env['CFLAGS'] = Split('-Os -Wall -g')
env.ParseConfig("pkg-config --cflags --libs glib-2.0")
testing = env.Clone()
testing['CPPPATH'] += ['build/cmockery/include/google', 'src']

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
objs = ['src/alias.o', 'src/helper.o']

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

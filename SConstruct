"""
Set version.
"""
#version = "0.1"

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
#env.Append(CFLAGS = ['-DVERSION=\\"%s\\"' % version])

"""
Set up install path.
"""
prefix  = env['prefix']
bin_dir = env['bindir'].replace('PREFIX', prefix)
man_dir = env['mandir'].replace('PREFIX', prefix)

"""
Compile the program.
"""
ticktock = env.Program('redshell', ['redshell.c', 'builtins.c'])

"""
Install the program.
"""
env.Install(bin_dir, ticktock)
env.Install(man_dir + "/man1", "ticktock.1")

env.Alias('install', prefix)

# vim: filetype=python

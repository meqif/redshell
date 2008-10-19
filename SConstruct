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

"""
Set up install path.
"""
prefix  = env['prefix']
bin_dir = env['bindir'].replace('PREFIX', prefix)
man_dir = env['mandir'].replace('PREFIX', prefix)

"""
Compile the program.
"""
redshell = env.Program('redshell', ['redshell.c', 'builtins.c', 'redir.c'])

"""
Install the program.
"""
env.Install(bin_dir, redshell)

env.Alias('install', prefix)

# vim: filetype=python

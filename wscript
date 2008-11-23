import Build, Configure, Options, Runner, Task, Utils

APPNAME='redshell'
VERSION='0.4'

srcdir = '.'
blddir = 'build'

sources = [
        'redshell.c', 'builtins.c', 'command.c', 'jobs.c', 'helper.c',
        'pipeline.c', 'alias.c' ]

def set_options(opt):
    pass

def configure(conf):
    conf.check_tool('gcc')
    conf.env['CCFLAGS'] = '-Os -Wall -g'

    conf.check_cfg(package='glib-2.0', atleast_version='2.6.0',
            uselib_store='GLIB', mandatory=True)
    conf.check_cfg(package='glib-2.0', args='--cflags --libs',
            uselib_store='GLIB')

def build(bld):
     obj = bld.new_task_gen('cc', 'program')
     obj.name           = 'redshell'
     obj.target         = 'redshell'
     obj.source         = sources
     obj.includes       = '.'
     obj.uselib         = 'GLIB'

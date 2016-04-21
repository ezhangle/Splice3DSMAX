#
# Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
#

import os, platform

Import('parentEnv')

buildOS = platform.system()
buildArch = 'x86_64'
buildType = 'Release'

scenegraph_dir = os.environ['FABRIC_SCENE_GRAPH_DIR']
prebuilt_dir = os.path.join(scenegraph_dir, 'ThirdParty', 'PreBuilt', buildOS, buildArch, 'Release', 'VS2013')

env = parentEnv.Clone()

stageDir = env.Dir(scenegraph_dir).Dir('stage').Dir(buildOS).Dir(buildArch).Dir(buildType)

maxVersions = ['2014', '2015', '2016']

qt_dir = os.path.join(prebuilt_dir, 'qt', '4.8.7')
env['ENV']['QT_DIR'] = qt_dir

qtwinmigrate_dir = os.path.join(prebuilt_dir, 'qt-solutions', 'qtwinmigrate')
env['ENV']['QTWINMIGRATE_DIR'] = qtwinmigrate_dir

# reset the Max SDK so it gets the one in our repo, just in case :)
env['ENV']['MaxSDKPath'] = ''

maxFiles = []
#'/property:Configuration=Release 2016',
for maxVersion in maxVersions:
    maxBuild = env.Command(
        'build max '+maxVersion,
        [],
        [
            ['C:\\Program Files (x86)\\MSBuild\\12.0\\Bin\\MSBuild.exe', '/m:8', '/p:Configuration=Release ' + maxVersion, '/p:Platform=x64', os.path.join(scenegraph_dir,'Splice','Applications','Splice3DSMAX', 'Fabric3dsmax.sln')],
        ]
    )
    maxFiles.append(maxBuild)

    maxModuleDir = stageDir.Dir('DCCIntegrations').Dir('FabricMax'+maxVersion)
    maxModuleFile = env.Dir('.').File('FIXME-max-dll.dll')
    maxFiles.append(env.Install(maxModuleDir.Dir('FIXME-plug-in', maxModuleFile)))

alias = env.Alias('splicemax', maxFiles)
spliceData = (alias, maxFiles)
Return('spliceData')

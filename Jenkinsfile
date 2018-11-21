@Library('rmc-jenkins-libraries') _

def parallelCtx = rmcBuild.parallelContext({ ->
    """osl42-x86_64
    winxp-x86-mingw32
    winxp-x86_64-mingw32
    sled11-x86-gcc4.8
    sled11-x86_64-gcc4.8
    ubuntu12.04-armhf-gcc4.7
    ubuntu14.04-armhf-gcc4.9
    vxworks6.7-x86-gcc4.1
    vxworks6.8-x86-gcc4.1
    vxworks6.9-x86-gcc4.3
    vxworks6.9.3-x86-gcc4.3
    vxworks6.9.3-armv7-gcc4.3
    vxworks6.9.4-x86-gcc4.3
    vxworks7.0.0-x86_64-gcc4.8
    qnx6.3-x86-gcc3.3
    qnx6.5-x86-gcc4.4
    qnx6.5-x86-gcc4.4-gpp
    qnx6.4-powerpc-gcc4.3
    android17-armhf-gcc4.9
    android17-x86-gcc4.9
    android21-aarch64-gcc4.9
    """})
parallelCtx.checkout()
parallelCtx.generateContext { -> conan.init() }

parallelCtx.stage('create') { vars ->
    conan.pkgCreate(vars.ctx, vars.profile)
}

parallelCtx.stage('upload') { vars ->
    if(vars.ctx.repoContext.isTagFromBranch('master') || vars.ctx.repoContext.isReleaseBranch()){
        conan.pkgUpload(vars.ctx)
    } else {
        println 'Skipping upload of Snapshot'
    }
}

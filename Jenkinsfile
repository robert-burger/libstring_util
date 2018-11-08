@Library('rmc-jenkins-libraries') _

String profiles = rmcBuild.findProfiles()
println("Building for: $profiles")

def parallelCtx = rmcBuild.parallelContext(profiles)
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

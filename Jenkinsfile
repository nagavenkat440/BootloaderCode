
stage('Clean Workspace') {
    steps {
        deleteDir()
    }
}

pipeline {
    agent any

    stages {

        stage('Checkout') {
            steps {
                checkout scm
            }
        }

        stage('Build STM32') {
    steps {
        bat '''
        set STM_WS=%WORKSPACE%\\stm_workspace

        if exist "%STM_WS%" rmdir /s /q "%STM_WS%"

        "C:\\ST\\STM32CubeIDE_1.15.1\\STM32CubeIDE\\stm32cubeidec.exe" ^
        -data "%STM_WS%" ^
        -nosplash ^
        -application org.eclipse.cdt.managedbuilder.core.headlessbuild ^
        -importAll "%WORKSPACE%" ^
        -cleanBuild "BootloaderCode/Release"
        '''
    }
}

        stage('Verify Artifacts') {
            steps {
                bat 'dir Release'
            }
        }
    }

    post {
        success {
            archiveArtifacts artifacts: 'Release/*.elf, Release/*.hex, Release/*.bin'
        }
    }
}
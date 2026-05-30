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
                "C:\\ST\\STM32CubeIDE_1.15.1\\STM32CubeIDE\\stm32cubeidec.exe" ^
                -data C:\\JenkinsWorkspace ^
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
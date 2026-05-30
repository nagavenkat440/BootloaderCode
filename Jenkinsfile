

pipeline {
    agent any

    stages {

        stage('Clean Workspace') {
            steps {
                deleteDir()
            }
        }

        stage('Checkout') {
            steps {
                checkout scm
            }
        }

        stage('Build STM32') {
            steps {
                bat '''
                set PATH=C:\\ST\\STM32CubeIDE_1.15.1\\STM32CubeIDE\\plugins\\com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.12.3.rel1.win32_1.0.100.202403111256\\tools\\bin;%PATH%

                cd Release

                "C:\\ST\\STM32CubeIDE_1.15.1\\STM32CubeIDE\\plugins\\com.st.stm32cube.ide.mcu.externaltools.make.win32_2.1.300.202402091052\\tools\\bin\\make.exe" clean

                "C:\\ST\\STM32CubeIDE_1.15.1\\STM32CubeIDE\\plugins\\com.st.stm32cube.ide.mcu.externaltools.make.win32_2.1.300.202402091052\\tools\\bin\\make.exe" -j16 all
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
            archiveArtifacts artifacts: 'Release/*.elf,Release/*.hex,Release/*.bin,Release/*.map'
        }
    }
}
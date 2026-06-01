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
            set PATH=C:\\ST\\STM32CubeIDE_1.15.1\\STM32CubeIDE\\plugins\\com.st.stm32cube.ide.mcu.externaltools.make.win32_2.1.300.202402091052\\tools\\bin;%PATH%
            set PATH=C:\\ST\\STM32CubeIDE_1.15.1\\STM32CubeIDE\\plugins\\com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.12.3.rel1.win32_1.0.100.202403111256\\tools\\bin;%PATH%

            where make
            where arm-none-eabi-gcc

            cd Release

            make clean
            make -j16 all
            '''
        }
    }

    stage('Build Info') {
        steps {
            echo "Build Number: ${BUILD_NUMBER}"
            echo "Git Commit: ${GIT_COMMIT}"
        }
    }

    stage('Verify Artifacts') {
        steps {
            bat 'dir Release'
        }
    }

    stage('Parasoft Check') {
        steps {
            bat 'cpptestcli -version'
        }
    }

    stage('ST-LINK Test') {
        steps {
            bat '''
            "C:\\ST\\STM32CubeIDE_1.15.1\\STM32CubeIDE\\plugins\\com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.win32_2.1.201.202404072231\\tools\\bin\\STM32_Programmer_CLI.exe" ^
            -l stlink
            '''
        }
    }

    stage('Flash STM32') {
        steps {
            catchError(buildResult: 'SUCCESS', stageResult: 'UNSTABLE') {
                bat '''
                "C:\\ST\\STM32CubeIDE_1.15.1\\STM32CubeIDE\\plugins\\com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.win32_2.1.201.202404072231\\tools\\bin\\STM32_Programmer_CLI.exe" ^
                -c port=SWD ^
                -w Release\\BootloaderCode.hex ^
                -v ^
                -rst
                '''
            }
        }
    }
}

post {
    success {
        archiveArtifacts artifacts: 'Release/*.elf,Release/*.hex,Release/*.bin,Release/*.map,Release/*.list'
    }

    always {
        echo 'Pipeline Finished'
    }
}


}

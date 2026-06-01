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

            echo ===== TOOL CHECK =====

            where make
            where arm-none-eabi-gcc

            make --version
            arm-none-eabi-gcc --version

            echo ===== BUILD =====

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

    stage('Static Analysis') {
        steps {
            bat '''
            cpptestcli -version
            '''
        }
    }

    stage('ST-LINK Test') {
        steps {
            bat '''
            echo ===== ST-LINK TEST =====

            "C:\\ST\\STM32CubeIDE_1.15.1\\STM32CubeIDE\\plugins\\com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.win32_2.1.201.202404072231\\tools\\bin\\STM32_Programmer_CLI.exe" ^
            -l stlink
            '''
        }
    }

    stage('Unit Tests') {
        steps {
            echo 'Running Unit Tests'
        }
    }

    stage('Flash STM32') {
        steps {
            bat '''
            echo ===== ST-LINK DETECTION =====

            "C:\\ST\\STM32CubeIDE_1.15.1\\STM32CubeIDE\\plugins\\com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.win32_2.1.201.202404072231\\tools\\bin\\STM32_Programmer_CLI.exe" ^
            -l stlink

            echo ===== FLASHING =====

            "C:\\ST\\STM32CubeIDE_1.15.1\\STM32CubeIDE\\plugins\\com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.win32_2.1.201.202404072231\\tools\\bin\\STM32_Programmer_CLI.exe" ^
            -c port=SWD ^
            -w Release\\BootloaderCode.hex ^
            -v ^
            -rst
            '''
        }
    }
}

post {
    success {
        archiveArtifacts artifacts: 'Release/*.elf,Release/*.hex,Release/*.bin,Release/*.map,Release/*.list'
    }

    failure {
        echo 'Build Failed'
    }
}


}

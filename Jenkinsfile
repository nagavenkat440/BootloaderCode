pipeline {
agent any

 environment {
    STM32_MAKE = 'C:\\ST\\STM32CubeIDE_2.0.0\\STM32CubeIDE\\plugins\\com.st.stm32cube.ide.mcu.externaltools.make.win32_2.2.0.202409170845\\tools\\bin'

    STM32_GCC = 'C:\\ST\\STM32CubeIDE_2.0.0\\STM32CubeIDE\\plugins\\com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.win32_1.0.100.202509120712\\tools\\bin'

    STM32_PROG = 'C:\\ST\\STM32CubeIDE_2.0.0\\STM32CubeIDE\\plugins\\com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.win32_2.2.300.202508131133\\tools\\bin\\STM32_Programmer_CLI.exe'
}


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
        set PATH=%STM32_MAKE%;%STM32_GCC%;%PATH%

        where make
        where arm-none-eabi-gcc

        make --version
        arm-none-eabi-gcc --version

        cd Release

        make clean
        make -j16 all
        '''
    }
}

stage('Generate BDF') {
    steps {
        bat '''
        set PATH=%STM32_MAKE%;%STM32_GCC%;%PATH%

        cd Release

        cpptesttrace ^
        --cpptesttraceOutputFile=cpptest.bdf ^
        make clean

        cpptesttrace ^
        --cpptesttraceOutputFile=cpptest.bdf ^
        make -j16 all

        dir cpptest.bdf
        '''
    }
}

stage('Parasoft Static Analysis') {
    steps {
        catchError(buildResult: 'SUCCESS', stageResult: 'UNSTABLE') {
            bat '''
            set PATH=%STM32_MAKE%;%STM32_GCC%;%PATH%

            cd Release

            echo bdf.import.compiler.family=gcc_11-64 > localsettings.properties
            echo bdf.import.c.compiler.exec=arm-none-eabi-gcc >> localsettings.properties
            echo bdf.import.cpp.compiler.exec=arm-none-eabi-g++ >> localsettings.properties
            echo bdf.import.linker.exec=arm-none-eabi-gcc >> localsettings.properties

            type localsettings.properties
            dir cpptest.bdf

            cpptestcli ^
            -config "builtin://Recommended Rules" ^
            -bdf cpptest.bdf ^
            -settings localsettings.properties ^
            -report reports ^
            -showdetails

            if exist reports (
                echo Static Analysis Report Generated
                dir reports
            ) else (
                echo Static Analysis Report NOT Generated
            )
            '''
        }
    }
}
    stage('Build Info') {
        steps {
            script {
                currentBuild.description =
                    "Build #${BUILD_NUMBER} - ${GIT_COMMIT.take(8)}"
            }
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

    stage('Build Unit Tests') {
        steps {
            bat '''
            if exist build rmdir /S /Q build

            cmake -S . -B build ^
            -G "MinGW Makefiles" ^
            -DCMAKE_C_COMPILER=C:/Qt/Qt5.12.12/Tools/mingw730_64/bin/gcc.exe ^
            -DCMAKE_CXX_COMPILER=C:/Qt/Qt5.12.12/Tools/mingw730_64/bin/g++.exe

            cmake --build build
            '''
        }
    }

    stage('Run Unit Tests') {
        steps {
            bat '''
            build\\runTests.exe --gtest_output=xml:test_results.xml
            '''
        }
    }

    stage('Code Coverage') {
        steps {
            bat '''
            python -m gcovr ^
            -r . ^
            --filter "Core/Src/.*" ^
            --exclude "googletest/.*" ^
            --html ^
            --html-details ^
            -o coverage.html
            '''
        }
    }

    stage('Publish Coverage') {
        steps {
            publishHTML([
                allowMissing: false,
                alwaysLinkToLastBuild: true,
                keepAll: true,
                reportDir: '.',
                reportFiles: 'coverage.html',
                reportName: 'STM32 Coverage Report'
            ])
        }
    }

stage('Verify CubeProgrammer') {
    steps {
        bat '''
        dir "%STM32_PROG%"
        "%STM32_PROG%" --version
        '''
    }
}

    stage('ST-LINK Test') {
    steps {
        bat '"%STM32_PROG%" -l stlink'
    }
}

    stage('Flash STM32') {
    steps {
        bat '''
        "%STM32_PROG%" ^
        -c port=SWD ^
        -w Release\\BootloaderCode.hex ^
        -v ^
        -rst
        '''
    }
}
}

post {
    always {
        junit allowEmptyResults: true,
              testResults: 'test_results.xml'

        archiveArtifacts allowEmptyArchive: true,
                         artifacts: '''
                         coverage.html,
                         test_results.xml,
                         Release/*.elf,
                         Release/*.hex,
                         Release/*.bin,
                         Release/*.map,
                         Release/*.list,
                         Release/cpptest.bdf,
                         Release/reports/**
                         '''
    }

    failure {
        echo 'Pipeline Failed'
    }
}


}

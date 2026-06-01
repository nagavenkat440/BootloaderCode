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

                cd Release
                make clean
                make -j16 all
                '''
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

        stage('ST-LINK Test') {
            steps {
                bat '''
                "C:\\ST\\STM32CubeIDE_1.15.1\\STM32CubeIDE\\plugins\\com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.win32_2.1.201.202404072231\\tools\\bin\\STM32_Programmer_CLI.exe" -l stlink
                '''
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

    } // stages

    post {

        always {
            junit 'test_results.xml'
        }

        success {
            archiveArtifacts artifacts: '''
                coverage.html,
                test_results.xml,
                Release/*.elf,
                Release/*.hex,
                Release/*.bin,
                Release/*.map,
                Release/*.list
            '''
        }

        failure {
            echo 'Pipeline Failed'
        }
    }

} // pipeline
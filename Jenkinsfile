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
                bat 'cppcheck --version'
            }
        }

        stage('Unit Tests') {
            steps {
                echo 'Running Unit Tests'
            }
        }

        stage('Flash STM32') {
            steps {
                echo 'Flashing STM32'
            }
        }
    }

    post {
        success {
            archiveArtifacts artifacts: 'Release/*.elf,Release/*.hex,Release/*.bin,Release/*.map'
        }
    }
}
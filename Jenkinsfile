pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                sh "mkdir build || true; cd build && cmake .. && make -j `getconf _NPROCESSORS_ONLN`;"
            }
        }
        stage('Test') {
            steps {
                echo 'Testing..'
                sh "cd build && ctest -j `getconf _NPROCESSORS_ONLN` --test-load `getconf _NPROCESSORS_ONLN`"
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying.... (skip)'
            }
        }
    }
}

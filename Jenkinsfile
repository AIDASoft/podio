pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                sh """
                source init.sh && 
                mkdir build install || true && 
                cd build && 
                cmake -DCMAKE_INSTALL_PREFIX=../install .. && 
                make -j `getconf _NPROCESSORS_ONLN` install;
                """
            }
        }
        stage('Test') {
            steps {
                echo 'Testing..'
                sh """
                cd build && 
                ctest -j `getconf _NPROCESSORS_ONLN` --test-load `getconf _NPROCESSORS_ONLN`
                """
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying.... (skip)'
            }
        }
    }
}

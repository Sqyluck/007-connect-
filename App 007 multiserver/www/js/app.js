var app = angular.module('app', ['ngRoute'])

/** Configure redirection to partials **/
app.config(($routeProvider) => {
  $routeProvider
    .when('/main', {templateUrl: 'partials/main.html'})
    .when('/main/:replay', {templateUrl: 'partials/main.html'})
    .when('/game', {templateUrl: 'partials/game.html'})
    .when('/result', {templateUrl: 'partials/result.html'})
    .otherwise({redirectTo: '/main'})
})

/** Change content of header **/
app.controller('appCtrl', ['$scope', '$rootScope', ($scope, $rootScope) => {
  $rootScope.ipAddress = '10.10.10.10:80'
  $rootScope.idPlayer = -1
  $rootScope.idGame = -1
}])

app.controller('main', ['$scope', '$http', '$rootScope', '$timeout', '$location', '$routeParams', ($scope, $http, $rootScope, $timeout, $location, $routeParams) => {
  var joinGameRequest = () => {
    return new Promise((resolve, reject) => {
      var url = 'http://' + $rootScope.ipAddress + '/joinGame?idPlayer=' + $rootScope.idPlayer + '&idGame=' + $rootScope.idGame
      console.log(url)
      $http({
        method: 'GET',
        url: url
      }).then((response) => {
        resolve(response.data)
      }, (error) => {
        reject(error)
      })
    })
  }

  var startGameRequest = () => {
    return new Promise((resolve, reject) => {
      var url = 'http://' + $rootScope.ipAddress + '/gameStarted?idGame=' + $rootScope.idGame
      $http({
        method: 'GET',
        url: url
      }).then((response) => {
        resolve(response.data)
      }, (error) => {
        reject(error)
      })
    })
  }
  $scope.accessGame = false
  $scope.waitingGame = false
  $scope.joinGame = () => {
    joinGameRequest()
    .then((response) => {
      var res = response.split('-')
      $scope.accessGame = true
      $rootScope.life = parseInt(res[0])
      $rootScope.bullet = parseInt(res[1])
      $rootScope.idPlayer = parseInt(res[2])
      $rootScope.idGame = parseInt(res[3])
      $rootScope.myAction = 0
      console.log('idGame : ' + $rootScope.idGame + 'idPlayer : ' + $rootScope.idPlayer + ', life : ' + $rootScope.life + ', bullet : ' + $rootScope.bullet)
      $scope.waitingGame = true
      $timeout(startGame, 500)
    })
    .catch((error) => {
      console.log('Can\'t access game : ' + error)
    })
  }

  if ($routeParams.replay === '1') {
    $scope.joinGame()
    console.log('rejoindre une nouvelle partie')
  }

  var startGame = () => {
    startGameRequest()
    .then((response) => {
      console.log(response + ' : game ready')
      $location.path('/game')
      $scope.$apply()
    })
    .catch((error) => {
      console.log('waiting the other player' + error)
      $timeout(startGame, 500)
    })
  }
}])

app.controller('game', ['$scope', '$rootScope', '$http', '$timeout', '$location', ($scope, $rootScope, $http, $timeout, $location) => {
  $scope.showresult = false
  $scope.continue = false
  $scope.otherResult = 0

  $scope.action = (action) => {
    actionReqest(action)
    .then((response) => {
      $rootScope.myAction = response
      $scope.$apply()
      console.log('action autorisé ' + response)
      $timeout(getResult, 500)
    })
    .catch((error) => {
      console.log('Action impossible ' + error)
    })
  }

  var getResult = () => {
    getResultRequest()
    .then((response) => {
      var res = response.split('-')
      $rootScope.life = parseInt(res[0])
      $rootScope.bullet = parseInt(res[1])
      $rootScope.result = parseInt(res[2])
      $scope.otherResult = parseInt(res[3])
      $scope.showresult = true
      if ($scope.result === 2) {
        console.log('player' + ($rootScope.idPlayer + 1) + ' a gagné')
        $location.path('/result')
      } else if ($scope.result === 0) {
        console.log('player' + ($rootScope.idPlayer + 1) + ' a perdu')
        $location.path('/result')
      } else {
        $scope.continue = true
      }
      $scope.$apply()
    })
    .catch((error) => {
      console.log('waiting others to play ' + error)
      $timeout(getResult, 500)
    })
  }

  $scope.continuer = () => {
    $rootScope.myAction = 0
    $scope.otherResult = 0
    $scope.continue = false
  }

  var actionReqest = (action) => {
    return new Promise((resolve, reject) => {
      var url = 'http://' + $rootScope.ipAddress + '/action?idPlayer=' + $rootScope.idPlayer + '&action=' + action + '&idGame=' + $rootScope.idGame
      $http({
        method: 'GET',
        url: url
      }).then((response) => {
        resolve(response.data)
      }, (error) => {
        reject(error)
      })
    })
  }

  var getResultRequest = () => {
    return new Promise((resolve, reject) => {
      var url = 'http://' + $rootScope.ipAddress + '/getResult?idPlayer=' + $rootScope.idPlayer + '&idGame=' + $rootScope.idGame
      $http({
        method: 'GET',
        url: url
      }).then((response) => {
        console.log(response.data)
        resolve(response.data)
      }, (error) => {
        reject(error)
      })
    })
  }
}])

app.controller('result', ['$scope', '$rootScope', '$location', '$http', ($scope, $rootScope, $location, $http) => {
  $scope.replay = () => {
    $location.path('/main/1')
  }
  $scope.quit = () => {
    quitRequest()
    .then((response) => {
      $rootScope.idPlayer = -1
      $rootScope.idGame = -1
      console.log('partie quittée')
      $location.path('/main')
      $scope.$apply()
    })
    .catch((err) => {
      console.log(err)
    })
  }

  var quitRequest = () => {
    return new Promise((resolve, reject) => {
      var url = 'http://' + $rootScope.ipAddress + '/quit?idPlayer=' + $rootScope.idPlayer + '&idGame=' + $rootScope.idGame
      $http({
        method: 'GET',
        url: url
      }).then((response) => {
        resolve(response.data)
      }, (error) => {
        reject(error)
      })
    })
  }
}])

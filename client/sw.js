var CACHE_NAME = 'my-pwa-cache-v1';
var urlsToCache = [
    './',
    './assets/',
    './images/',
    './manifest.json',
    './main.js'
];

self.addEventListener('install', function (event) {
    // Perform install steps
    event.waitUntil(
        caches.open(CACHE_NAME)
            .then(async function (cache) {
                console.log('Opened cache');
                return cache.addAll(urlsToCache);
            })
  );
  console.log("App Installable");
});
self.addEventListener('activate', event => {
    event.waitUntil(self.clients.claim());
});
self.addEventListener('fetch', function (event) {
    event.respondWith(
        caches.match(event.request)
          .then(function(response) {
            // Cache hit - return response
            if (response) {
              return response;
            }
            return fetch(event.request);
          }
        )
      );
  console.log("fetch");
});
'use strict';
const MANIFEST = 'flutter-app-manifest';
const TEMP = 'flutter-temp-cache';
const CACHE_NAME = 'flutter-app-cache';
const RESOURCES = {
  "assets/AssetManifest.json": "56e8dc5e25b482b2b87b5cc133ef66fb",
"assets/assets/icons/temp.txt": "d41d8cd98f00b204e9800998ecf8427e",
"assets/assets/images/pasleto.jpg": "d23ca9d415498c29eab0f0904159f0ed",
"assets/assets/images/sentinel.png": "3775b403145c9c81ea20e77c49121de1",
"assets/assets/images/temp.txt": "d41d8cd98f00b204e9800998ecf8427e",
"assets/assets/logo/Logo_1024.png": "1a1cbc4daa701f7a4c5e8d1f708c5049",
"assets/assets/logo/Logo_128.png": "9a9b029c57e3f8df9202c041e0f36cf6",
"assets/assets/logo/Logo_16.png": "0b9f9457827dc320be35287ad7d3e15e",
"assets/assets/logo/Logo_256.png": "0eae06abb24058b3674978aa68dd2d94",
"assets/assets/logo/Logo_32.png": "c4e28a651dd8a5098a94bef9810a47b1",
"assets/assets/logo/Logo_512.png": "d166bab78aa036fb16dba7f7b25703ca",
"assets/assets/logo/Logo_64.png": "82bcddfc5f38ffbe3db795308ffaac07",
"assets/assets/logo/Logo_800.png": "e44ee42a9f745fa062fefa45cd04c585",
"assets/assets/logo/Logo_Gray_1024.png": "b66b241119eb007a1f0e1ace9a3dae37",
"assets/assets/logo/Logo_Gray_128.png": "83089cb9c7a71962920bad0b18fdac88",
"assets/assets/logo/Logo_Gray_16.png": "ea9a8b39e07e32329c4c625473ab68f5",
"assets/assets/logo/Logo_Gray_256.png": "dfc5488a0e93fed08024982fb6b75d0f",
"assets/assets/logo/Logo_Gray_32.png": "042fb7113f7b12d27090a3c65d551808",
"assets/assets/logo/Logo_Gray_512.png": "7344c8d48d43a0d2c26af9e9423751c6",
"assets/assets/logo/Logo_Gray_64.png": "0a3275d145ddcf0d8695a202cacc2065",
"assets/assets/logo/Logo_Gray_800.png": "637f4898627fcef84973dfdac4abeaa0",
"assets/FontManifest.json": "b417c8197e08be3f18101f9b01bf0e45",
"assets/fonts/MaterialIcons-Regular.otf": "95db9098c58fd6db106f1116bae85a0b",
"assets/NOTICES": "9112fc1c894357e31f8d2ecad43f7891",
"assets/packages/cupertino_icons/assets/CupertinoIcons.ttf": "6d342eb68f170c97609e9da345464e5e",
"assets/packages/fluent_ui/assets/AcrylicNoise.png": "81f27726c45346351eca125bd062e9a7",
"assets/packages/fluent_ui/fonts/FluentIcons.ttf": "1cd173aed13e298ab2663dd0924f6762",
"assets/shaders/ink_sparkle.frag": "ae6c1fd6f6ee6ee952cde379095a8f3f",
"canvaskit/canvaskit.js": "2bc454a691c631b07a9307ac4ca47797",
"canvaskit/canvaskit.wasm": "bf50631470eb967688cca13ee181af62",
"canvaskit/profiling/canvaskit.js": "38164e5a72bdad0faa4ce740c9b8e564",
"canvaskit/profiling/canvaskit.wasm": "95a45378b69e77af5ed2bc72b2209b94",
"favicon.ico": "27a96aab724607fcb5eac65f13c9ba8f",
"favicon.png": "5dcef449791fa27946b3d35ad8803796",
"flutter.js": "f85e6fb278b0fd20c349186fb46ae36d",
"icons/Icon-192.png": "b8b9c396f628da963951f5ae6ef9df7d",
"icons/Icon-512.png": "32235482c698c59c937e6f556d52c48a",
"icons/Icon-maskable-192.png": "c4cfb182f60f76d37dc0153a302d9410",
"icons/Icon-maskable-512.png": "c730d2cea1befbfff8128bf7f69d295b",
"index.html": "76224a01caacc65c41440e08ad32f3d1",
"/": "76224a01caacc65c41440e08ad32f3d1",
"main.dart.js": "127ca307289e08189b500ebd082af63c",
"main.dart.js_1.part.js": "108c1b680164758ac2b7b210a50c9cce",
"main.dart.js_2.part.js": "df4089b7eb2fa96eff28337896be2b38",
"main.dart.js_3.part.js": "fe1ab32859c9762e3c45807914a6dd8f",
"main.dart.js_4.part.js": "dba7c86e7be1a99e2fd5d0d9960425f1",
"main.dart.js_5.part.js": "250115da64158e3c6e456c57c93b8891",
"main.dart.js_6.part.js": "5c0ba278fe3485961e5701d11c0d651e",
"main.dart.js_7.part.js": "12cd2298d9cba22da37ab55439493f00",
"main.dart.js_8.part.js": "2b623479e7e0cbd23c1a371d401e7445",
"manifest.json": "e6fa0434a61aa4a184fd6825b34aa3ad",
"splash/img/dark-1x.png": "a211ccd77e9e65ab4d9d481996169a96",
"splash/img/dark-2x.png": "916cc1f78a97b2e5d1471df826072d34",
"splash/img/dark-3x.png": "19b5605737420c01948d8c8c56e34265",
"splash/img/light-1x.png": "a211ccd77e9e65ab4d9d481996169a96",
"splash/img/light-2x.png": "916cc1f78a97b2e5d1471df826072d34",
"splash/img/light-3x.png": "19b5605737420c01948d8c8c56e34265",
"splash/style.css": "123ffdaf0b29e310625d175083b70252",
"version.json": "513f3802abfa874e00a7c49cd0f92558"
};

// The application shell files that are downloaded before a service worker can
// start.
const CORE = [
  "main.dart.js",
"index.html",
"assets/AssetManifest.json",
"assets/FontManifest.json"];
// During install, the TEMP cache is populated with the application shell files.
self.addEventListener("install", (event) => {
  self.skipWaiting();
  return event.waitUntil(
    caches.open(TEMP).then((cache) => {
      return cache.addAll(
        CORE.map((value) => new Request(value, {'cache': 'reload'})));
    })
  );
});

// During activate, the cache is populated with the temp files downloaded in
// install. If this service worker is upgrading from one with a saved
// MANIFEST, then use this to retain unchanged resource files.
self.addEventListener("activate", function(event) {
  return event.waitUntil(async function() {
    try {
      var contentCache = await caches.open(CACHE_NAME);
      var tempCache = await caches.open(TEMP);
      var manifestCache = await caches.open(MANIFEST);
      var manifest = await manifestCache.match('manifest');
      // When there is no prior manifest, clear the entire cache.
      if (!manifest) {
        await caches.delete(CACHE_NAME);
        contentCache = await caches.open(CACHE_NAME);
        for (var request of await tempCache.keys()) {
          var response = await tempCache.match(request);
          await contentCache.put(request, response);
        }
        await caches.delete(TEMP);
        // Save the manifest to make future upgrades efficient.
        await manifestCache.put('manifest', new Response(JSON.stringify(RESOURCES)));
        return;
      }
      var oldManifest = await manifest.json();
      var origin = self.location.origin;
      for (var request of await contentCache.keys()) {
        var key = request.url.substring(origin.length + 1);
        if (key == "") {
          key = "/";
        }
        // If a resource from the old manifest is not in the new cache, or if
        // the MD5 sum has changed, delete it. Otherwise the resource is left
        // in the cache and can be reused by the new service worker.
        if (!RESOURCES[key] || RESOURCES[key] != oldManifest[key]) {
          await contentCache.delete(request);
        }
      }
      // Populate the cache with the app shell TEMP files, potentially overwriting
      // cache files preserved above.
      for (var request of await tempCache.keys()) {
        var response = await tempCache.match(request);
        await contentCache.put(request, response);
      }
      await caches.delete(TEMP);
      // Save the manifest to make future upgrades efficient.
      await manifestCache.put('manifest', new Response(JSON.stringify(RESOURCES)));
      return;
    } catch (err) {
      // On an unhandled exception the state of the cache cannot be guaranteed.
      console.error('Failed to upgrade service worker: ' + err);
      await caches.delete(CACHE_NAME);
      await caches.delete(TEMP);
      await caches.delete(MANIFEST);
    }
  }());
});

// The fetch handler redirects requests for RESOURCE files to the service
// worker cache.
self.addEventListener("fetch", (event) => {
  if (event.request.method !== 'GET') {
    return;
  }
  var origin = self.location.origin;
  var key = event.request.url.substring(origin.length + 1);
  // Redirect URLs to the index.html
  if (key.indexOf('?v=') != -1) {
    key = key.split('?v=')[0];
  }
  if (event.request.url == origin || event.request.url.startsWith(origin + '/#') || key == '') {
    key = '/';
  }
  // If the URL is not the RESOURCE list then return to signal that the
  // browser should take over.
  if (!RESOURCES[key]) {
    return;
  }
  // If the URL is the index.html, perform an online-first request.
  if (key == '/') {
    return onlineFirst(event);
  }
  event.respondWith(caches.open(CACHE_NAME)
    .then((cache) =>  {
      return cache.match(event.request).then((response) => {
        // Either respond with the cached resource, or perform a fetch and
        // lazily populate the cache only if the resource was successfully fetched.
        return response || fetch(event.request).then((response) => {
          if (response && Boolean(response.ok)) {
            cache.put(event.request, response.clone());
          }
          return response;
        });
      })
    })
  );
});

self.addEventListener('message', (event) => {
  // SkipWaiting can be used to immediately activate a waiting service worker.
  // This will also require a page refresh triggered by the main worker.
  if (event.data === 'skipWaiting') {
    self.skipWaiting();
    return;
  }
  if (event.data === 'downloadOffline') {
    downloadOffline();
    return;
  }
});

// Download offline will check the RESOURCES for all files not in the cache
// and populate them.
async function downloadOffline() {
  var resources = [];
  var contentCache = await caches.open(CACHE_NAME);
  var currentContent = {};
  for (var request of await contentCache.keys()) {
    var key = request.url.substring(origin.length + 1);
    if (key == "") {
      key = "/";
    }
    currentContent[key] = true;
  }
  for (var resourceKey of Object.keys(RESOURCES)) {
    if (!currentContent[resourceKey]) {
      resources.push(resourceKey);
    }
  }
  return contentCache.addAll(resources);
}

// Attempt to download the resource online before falling back to
// the offline cache.
function onlineFirst(event) {
  return event.respondWith(
    fetch(event.request).then((response) => {
      return caches.open(CACHE_NAME).then((cache) => {
        cache.put(event.request, response.clone());
        return response;
      });
    }).catch((error) => {
      return caches.open(CACHE_NAME).then((cache) => {
        return cache.match(event.request).then((response) => {
          if (response != null) {
            return response;
          }
          throw error;
        });
      });
    })
  );
}

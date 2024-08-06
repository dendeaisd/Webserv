### Nginx Feature list

You can pick any of these features and do an extensive research on how it works and more details about it.

1. HTTP server:
   Basic syntax:
   ```nginx
   http {
     server {
       listen 80;
       server_name example.com;
       root /var/www/html;
       index index.html;
     }
   }
   ```

2. HTTPS/SSL support:
   ```nginx
   server {
     listen 443 ssl;
     server_name example.com;
     ssl_certificate /path/to/cert.pem;
     ssl_certificate_key /path/to/key.pem;
   }
   ```

3. Reverse proxy:
   ```nginx
   location / {
     proxy_pass http://backend_server;
     proxy_set_header Host $host;
     proxy_set_header X-Real-IP $remote_addr;
   }
   ```

4. Load balancing:
   ```nginx
   upstream backend {
     server backend1.example.com;
     server backend2.example.com;
     server backend3.example.com;
   }
   server {
     location / {
       proxy_pass http://backend;
     }
   }
   ```

5. Caching:
   ```nginx
   proxy_cache_path /path/to/cache levels=1:2 keys_zone=my_cache:10m;
   server {
     location / {
       proxy_cache my_cache;
       proxy_cache_valid 200 60m;
       proxy_cache_use_stale error timeout http_500 http_502 http_503 http_504;
     }
   }
   ```

6. URL rewriting:
   ```nginx
   location /old-page {
     rewrite ^/old-page(.*)$ /new-page$1 permanent;
   }
   ```

7. Compression:
   ```nginx
   gzip on;
   gzip_types text/plain text/css application/json application/javascript text/xml application/xml application/xml+rss text/javascript;
   ```

8. Access control:
   ```nginx
   location /admin {
     allow 192.168.1.0/24;
     deny all;
   }
   ```

9. Logging:
   ```nginx
   access_log /var/log/nginx/access.log;
   error_log /var/log/nginx/error.log;
   ```

10. Virtual hosts:
    ```nginx
    server {
      listen 80;
      server_name site1.example.com;
      root /var/www/site1;
    }
    server {
      listen 80;
      server_name site2.example.com;
      root /var/www/site2;
    }
    ```

11. WebSocket support:
    ```nginx
    location /wsapp/ {
      proxy_pass http://wsbackend;
      proxy_http_version 1.1;
      proxy_set_header Upgrade $http_upgrade;
      proxy_set_header Connection "upgrade";
    }
    ```

12. FastCGI support:
    ```nginx
    location ~ \.php$ {
      fastcgi_pass unix:/var/run/php/php7.4-fpm.sock;
      fastcgi_index index.php;
      include fastcgi_params;
    }
    ```

13. uWSGI support:
    ```nginx
    location / {
      uwsgi_pass unix:/tmp/uwsgi.sock;
      include uwsgi_params;
    }
    ```

14. SCGI support:
    ```nginx
    location / {
      scgi_pass localhost:9000;
      include scgi_params;
    }
    ```

15. Streaming media (MP4/FLV):
    ```nginx
    location /videos/ {
      mp4;
      mp4_buffer_size 1m;
      mp4_max_buffer_size 5m;
    }
    ```

16. Rate limiting:
    ```nginx
    limit_req_zone $binary_remote_addr zone=mylimit:10m rate=10r/s;
    server {
      location /login/ {
        limit_req zone=mylimit burst=20 nodelay;
      }
    }
    ```

17. IP-based geolocation:
    ```nginx
    http {
      geoip_country /usr/share/GeoIP/GeoIP.dat;
      server {
        location / {
          if ($geoip_country_code = US) {
            return 302 https://us.example.com$request_uri;
          }
        }
      }
    }
    ```

18. HTTP/2 support:
    ```nginx
    server {
      listen 443 ssl http2;
      server_name example.com;
      ssl_certificate /path/to/cert.pem;
      ssl_certificate_key /path/to/key.pem;
    }
    ```

19. gRPC support:
    ```nginx
    server {
      listen 80 http2;
      location / {
        grpc_pass grpc://backend;
      }
    }
    ```

20. Content filtering:
    ```nginx
    location / {
      sub_filter 'href="http://example.com"' 'href="https://example.com"';
      sub_filter_once off;
    }
    ```

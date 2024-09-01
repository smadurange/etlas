ATLAS.FCGI

The atlas.fcgi file defines a FastCGI backend for Atlas. Follow the steps below to 
deploy Atlas to an OpenBSD server using httpd.

  1. Install flask, flup, and requests Python packages. 
  2. Create directory $HOME/atlas.
  3. Copy atlas.fcgi and tickers.txt to $HOME/atlas.
  4. Set the POLYGON_API_KEY environment variable: $ export POLYGON_API_KEY=<api-key>
  5. Start FastCGI server: $ nohup python3 $HOME/atlas/atlas.fcgi &
  6. Change ownership to /var/www/run/atlas.sock:
     # chown www /var/www/run/atlas.sock
     # chgrp www /var/www/run/atlas.sock
  7. Add the following configuration to httpd.conf:
     server "<server-name>" {
         listen on * port <port>
         location "/prices" {
             authenticate with "/htdocs/atlas/.htpasswd"
             fastcgi {
                 socket "/run/atlas.sock"
             }
         }
     }
  8. Create a user for the API: # htpasswd /var/www/htdocs/atlas/.htpasswd <username>
  9. Set read-only permissions to .htpasswd: # chmod u-w /var/www/htdocs/atlas/.htpasswd
  10. Start httpd: # rcctl -f start httpd 

ATLAS.FCGI

The atlas.fcgi file defines a FastCGI backend for Atlas. Follow the steps below to 
deploy Atlas to an OpenBSD server using httpd.

  1. Create directory $HOME/atlas.
  2. Copy atlas.fcgi and tickers.txt to $HOME/atlas.
  3. Set the POLYGON_API_KEY environment variable: $ export POLYGON_API_KEY=<api-key>
  4. Start FastCGI server: $ nohup python3 $HOME/atlas/atlas.fcgi &
  5. Change ownership to /var/www/run/atlas.sock:
     # chown www /var/www/run/atlas.sock
     # chgrp www /var/www/run/atlas.sock
  6. Add the following configuration to httpd.conf:
     server "<server-name>" {
         listen on * port <port>
         location "/prices" {
             authenticate with "/htdocs/atlas/.htpasswd"
             fastcgi {
                 socket "/run/atlas.sock"
             }
         }
     }
  7. Create a user for the API: # htpasswd $HOME/atlas/.htpasswd <username>
  8. Grant www access the password file: chmod +r $HOME/atlas/.htpasswd
  9. Start httpd: # rcctl -f start httpd 

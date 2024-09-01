ATLAS.FCGI

The atlas.fcgi file defines a FastCGI backend for Atlas. Follow the steps below to 
deploy Atlas to an OpenBSD server using httpd.

  1. Copy atlas.fcgi to the server and run the following commnad to install it to
     cgi-bin with execute permissions:
     # install -o www -g www -m 0500 atlas.fcgi /var/www/cgi-bin/atlas/atlas.fcgi
  2. Copy the tickers.txt file to /var/www/cgi-bin/atlas and make sure that the
     file is readable (-rw-r--r--).
  3. Set the POLYGON_API_KEY environment variable.
  4. Start FastCGI server: $ nohup python3 /var/www/cgi-bin/atlas/atlas.fcgi &
  5. Add the following configuration to httpd.conf:
     server "<server-name>" {
         listen on * port <port>
         location "/prices" {
             authenticate with "/htdocs/atlas/.htpasswd"
             fastcgi {
                 socket "/run/atlas.sock"
             }
         }
     }
  6. Create a user for basic authentication: # htpasswd /var/www/cgi-bin/atlas/.htpasswd <username>
  7. Grant permissions to www user to access the password file: chmod +r /var/www/cgi-bin/atlas/.htpasswd
  8. Start httpd.

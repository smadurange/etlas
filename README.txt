SERVER.FCGI

The server.fcgi file contains the server code for Atlas. Follow the steps below
to deploy Atlas to an OpenBSD server using httpd.

  1. Install flask, flup, and requests Python packages. 
  2. Create directory $HOME/atlas.
  3. Copy server.fcgi and tickers.txt to $HOME/atlas.
  4. Set the POLYGON_API_KEY environment variable.
  5. Start FastCGI server: $ nohup python3 $HOME/atlas/server.fcgi &
  6. Change ownership of /var/www/run/atlas.sock to www:
     # chown www /var/www/run/atlas.sock
     # chgrp www /var/www/run/atlas.sock
  7. Configure httpd (see scripts/httpd.conf for a template).
  8. Create API user:
     # mkdir /var/www/htdocs/atlas
     # htpasswd /var/www/htdocs/atlas/.htpasswd <username>
  9. Set permissions for .htpasswd: 
     # chown www /var/www/htdocs/atlas/.htpasswd
     # chgrp www /var/www/htdocs/atlas/.htpasswd
     # chmod u-w /var/www/htdocs/atlas/.htpasswd
  10. Start httpd: # rcctl -f start httpd 

FROM scratch
ADD bin /bin
ADD lib /lib
ADD lib64 /lib64
RUN echo '#!/bin/sh' > /entrypoint.sh
RUN echo 'exec /bin/tcp 12345 &' >> /entrypoint.sh
RUN echo 'exec /bin/tcp 127.0.0.1 12345 &' >> /entrypoint.sh
RUN echo 'exec /bin/buffer_performance_dpr $*' >> /entrypoint.sh
RUN chmod +x /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"] 

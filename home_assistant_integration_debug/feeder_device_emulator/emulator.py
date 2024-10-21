import json
import random
from http.server import *
from socketserver import BaseServer

class FeederConfigStorage:
    def __init__(self) -> None:        
        self.config = {
            'feed_speed': 1000,
            'feed_amount': 100,
            'steps_frw': 9,
            'steps_bkw': 3
        }
        self.info = {
            'model': 'Test Device',
            'version': f'0.0.{random.randint(1, 9)}',
            'manufacturer': 'Segen Nikita'
        }

configStorage = FeederConfigStorage()

# creating a class for handling  
# basic Get and Post Requests 
class FeederAPI(BaseHTTPRequestHandler): 
    
    def __init__(self, request, client_address, server: BaseServer) -> None:
        self.get_methods = {
            '/feedConfig': self.get_config,
            '/deviceInfo': self.get_info
        }
        self.post_methods = {
            '/feedConfig': self.save_config,
            '/feed': self.feed
        }
        super().__init__(request, client_address, server)
        
    def get_info(self):
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()
        self.wfile.write(bytes(json.dumps(configStorage.info), 'utf-8'))

    def get_config(self):
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()
        self.wfile.write(bytes(json.dumps(configStorage.config), 'utf-8'))

    def feed(self):
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()

    def save_config(self):
        length = int(self.headers.get('content-length'))
        field_data = self.rfile.read(length)
        configStorage.config = json.loads(str(field_data, 'utf-8'))
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()
    
    def do_GET(self): 
        if self.path not in self.get_methods:
            self.send_response(404)
            self.end_headers()
            return
        
        self.get_methods[self.path]()

    def do_POST(self): 
        if self.path not in self.post_methods:
            self.send_response(404)
            self.end_headers()
            return
        
        self.post_methods[self.path]()
  
if __name__ == "__main__": 
    host_name = ''
    port = 5555     
    webServer = HTTPServer((host_name, port), FeederAPI)
    print("Server started http://%s:%s" % (host_name, port))

    try:
        webServer.serve_forever()
    except KeyboardInterrupt:
        pass

    webServer.server_close()
    print("Server stopped.")
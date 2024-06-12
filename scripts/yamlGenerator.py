
import sys

MAX_CPU=8   # Maximum allocation of CPU (8 cores) 
MAX_MEM=8000 # Maximum allocation of RAM (8000MB)

def generate_docker_compose(config_file, signature_alg):
    with open(config_file, 'r') as f:
        lines = f.readlines()

    docker_compose_yaml = f"""version: "3.9"

networks:
  pqb_net:
    driver: bridge
    ipam:
      driver: default
      config:
        - subnet: 10.5.0.0/24
          gateway: 10.5.0.1

x-defaults: &pqb_base
  image: pqb_image
  deploy:
    resources:
      limits:
        cpus: "{MAX_CPU/len(lines)}"
        memory: "{MAX_MEM/len(lines)}M"
  privileged: true
  stdin_open: true
  tty: true

services:\n"""

    for idx, line in enumerate(lines, start=1):
        ip_address, *_ = line.split()
        node_name = f"node{idx}"
        port = 45000 + idx

        service_block = f"""
  {node_name}:
    <<: *pqb_base
    container_name: {node_name}
    command: ["sh", "-c", "./acc-generator < tmp/confs/confs.txt && ./commandRunner tmp/commands.txt {node_name} > tmp/pipe & perf stat ./pqb -s {signature_alg} -c tmp/confs/conf{idx}.json < tmp/pipe"]
    ports:
      - "{port}:3330"
    networks:
      pqb_net:
        ipv4_address: {ip_address}\n"""

        docker_compose_yaml += service_block

    return docker_compose_yaml

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py <config_file> <signature_algorithm_name>")
        sys.exit(1)

    config_file = sys.argv[1]
    signature_alg = sys.argv[2]
    docker_compose_yaml = generate_docker_compose(config_file, signature_alg)
    print(docker_compose_yaml)

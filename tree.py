import pygraphviz as pgv
import argparse

parser = argparse.ArgumentParser()

parser.add_argument('--nodes', type=str, required=True)
parser.add_argument('--edges', type=str, required=True)

args = parser.parse_args()

nodes = args.nodes
edges = args.edges

G=pgv.AGraph(directed=True)


edges_file = open(edges,'r')
edge_list = []

for line in edges_file.readlines():
    edge_list.append([int(a) for a in line.strip().split(',')])

label_file = open(nodes,'r')

labels = []
for line in label_file.readlines():
    labels = [int(a) for a in line.split(',')]

for a in labels:
	G.add_node(a,color='blue')

for a,b in edge_list:
	G.add_edge(b,a,color='blue')

G.write('test.dot')

G.layout(prog='dot')
G.draw('blockchain.png')
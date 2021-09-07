import networkx as nx
import numpy as np
import matplotlib.pyplot as plt
import argparse

def genGraph(n):
	G = nx.scale_free_graph(n)
	G.remove_edges_from(nx.selfloop_edges(G))
	G = G.to_undirected()
	return G

parser = argparse.ArgumentParser()

parser.add_argument('--n', type=int, required=True)

args = parser.parse_args()

n = args.n

G = genGraph(n)
while(not nx.is_connected(G)):
	G = genGraph(n)

pos = nx.circular_layout(G)
nx.draw(G,pos=pos)
plt.savefig('graph.png')

fp = open('G_node.txt','w')
fp.write(str(n)+'\n')
for a in G.edges():
	fp.write(str(a[0])+' '+str(a[1])+'\n')
import networkx as nx
import random
import matplotlib.pyplot as plt
    
# Function taken from https://newbedev.com/can-one-get-hierarchical-graphs-from-networkx-with-python-3 for a heirarchial blockchain tree generation and visualization   
def hierarchy_pos(G, root=None, width=1., vert_gap = 0.2, vert_loc = 0, xcenter = 0.5):

    if not nx.is_tree(G):
        raise TypeError('cannot use hierarchy_pos on a graph that is not a tree')

    if root is None:
        if isinstance(G, nx.DiGraph):
            root = next(iter(nx.topological_sort(G)))  #allows back compatibility with nx version 1.11
        else:
            root = random.choice(list(G.nodes))

    def _hierarchy_pos(G, root, width=1., vert_gap = 0.2, vert_loc = 0, xcenter = 0.5, pos = None, parent = None):

        if pos is None:
            pos = {root:(xcenter,vert_loc)}
        else:
            pos[root] = (xcenter, vert_loc)
        children = list(G.neighbors(root))
        if not isinstance(G, nx.DiGraph) and parent is not None:
            children.remove(parent)  
        if len(children)!=0:
            dx = width/len(children) 
            nextx = xcenter - width/2 - dx/2
            for child in children:
                nextx += dx
                pos = _hierarchy_pos(G,child, width = dx, vert_gap = vert_gap, 
                                    vert_loc = vert_loc-vert_gap, xcenter=nextx,
                                    pos=pos, parent = root)
        return pos

            
    return _hierarchy_pos(G, root, width, vert_gap, vert_loc, xcenter)


# Input edge list. root of tree taken as 0 by default
edge_list = list(tuple(map(int,input().split())) for r in range(int(input('enter no of rows : ')))) 
G=nx.Graph()
G.add_edges_from(edge_list)
pos = hierarchy_pos(G,0)    
nx.draw(G, pos=pos, with_labels=True)
plt.savefig('blockchain_tree.png')
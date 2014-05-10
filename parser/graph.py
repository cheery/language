import pygraphviz as pgv
import parser
import cgi # heh.
import sys

def main():
    if len(sys.argv) < 2:
        sourcefile = "sample"
        filename   = "sample.png"
    elif len(sys.argv) < 3:
        sourcefile = sys.argv[1]
        filename = "%s.png" % sourcefile
    else:
        sourcefile = sys.argv[1]
        filename   = sys.argv[2]

    graph = pgv.AGraph(directed=True, strict=False)
    graph.next_id = 1
    graph.cache   = {}

    graph.graph_attr['rankdir'] = 'TB'
    #graph.graph_attr['rankdir'] = 'LR'
    graph.node_attr['shape'] = 'Mrecord'

    for struct in parser.parse_file(sourcefile):
        populate_graph(graph, struct)
        print struct
    graph.draw(filename, prog="dot")

def populate_graph(graph, struct):
    if struct is None:
        return None
    uid = get_uid(graph, struct)
    if isinstance(struct, parser.Struct):
        for i, item in enumerate(struct):
            t = populate_graph(graph, item)
            if t is not None:
                graph.add_edge(uid, t, tailport='f%i'%i)#headport='<top>')
    return uid

def get_uid(graph, struct):
    if struct in graph.cache:
        return graph.cache[struct]
    if isinstance(struct, parser.Struct):
        label = "%s" % struct.group
        for i in range(len(struct)):
            label += "|<f%i> " % i
    else:
        label = cgi.escape(u"%s %s" % (struct.group, struct.value))
    uid = graph.next_id

    graph.cache[struct] = uid
    graph.add_node(uid, label=label)
    graph.next_id += 1
    return uid

#'neato','dot','twopi','circo','fdp','nop'

if __name__=='__main__':
    main()

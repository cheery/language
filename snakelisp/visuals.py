from cps import Call, Lambda, Assign, Variable, Constant, Environ, null, true, false
import pygraphviz as pgv

def create_graph(path, expr):
    graph = pgv.AGraph(directed=True, strict=False)
    graph.next_id = 1
    graph.cache   = {}

    graph.graph_attr['rankdir'] = 'TB'
    graph.node_attr['shape'] = 'Mrecord'

    populate(graph, expr)
    graph.draw(path, prog='dot')

def populate(graph, expr):
    if expr in graph.cache:
        return graph.cache[expr]
    if expr.type == 'lambda':
        lamb = get_uid(graph, expr, len(expr)+1)
        for var, val in expr.motion:
            uid = populate(graph, var)
            uid2 = populate(graph, val)
            new_edge(graph, lamb, len(expr), uid2)
            new_edge(graph, uid2, 0, uid)
        for i, var in enumerate(expr):
            uid = populate(graph, var)
            new_edge(graph, lamb, i, uid)
        body = populate(graph, expr.body)
        new_edge(graph, lamb, len(expr), body)
        return lamb
    if expr.type == 'call':
        call = get_uid(graph, expr, len(expr))
        for i, subexpr in enumerate(expr):
            uid = populate(graph, subexpr)
            new_edge(graph, call, i, uid)
        return call
    if expr.type == 'assign':
        assign = get_uid(graph, expr, 3)
        new_edge(graph, assign, 0, populate(graph, expr.variable))
        new_edge(graph, assign, 1, populate(graph, expr.value))
        new_edge(graph, assign, 2, populate(graph, expr.body))
        return assign
    if expr.type == 'variable':
        return get_uid(graph, expr, 1, {0:expr.name})
    return get_uid(graph, expr)

def new_edge(graph, a, port, b):
    graph.add_edge(a, b, tailport='f{}'.format(port))

def get_uid(graph, expr, arity=1, labels={}):
    if expr in graph.cache:
        return graph.cache[expr]
    label = expr.type
    for i in range(arity):
        label += "|<f{}> {}".format(i, labels.get(i, ''))
    uid   = graph.cache[expr] = graph.next_id
    graph.add_node(uid, label=label)
    graph.next_id += 1
    return uid


import itertools

def event_type(event):
    """Returns the field name in the Event which is not pid or tid; we
    expect this to look like 'trace_start', 'trace_end', etc."""
    top_level_required_fields = ['id', 'pid', 'tid', 'cross_ref']
    event_type = None
    for desc, val in event.ListFields():
        if desc.name in top_level_required_fields:
            continue
        assert event_type is None, \
            "Unknown message type: '%s' or '%s'?" % (event_type, desc.name)
        event_type = desc.name
    return event_type

def split_stream(generator, start_predicate):
    """Split a stream of items into chunks, based on
    start_predicate. For example, given a stream:
    x = [1, 2, 3, 1, 2, 1, 7, ...], where 1 is chosen
    as matching the start_predicate, this function will
    produce: [1, 2, 3], [1, 2], [1, 7], ..."""
    it, peek_it = itertools.tee(generator, 2)
    if not peek_it.next():
        return
    group = []
    while True:
        group.append(it.next())
        try:
            peek = peek_it.next()
        except StopIteration:
            break
        if start_predicate(peek):
            yield group
            group = []
    if group:
        yield group


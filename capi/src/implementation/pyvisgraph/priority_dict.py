import typing
from heapq import heapify, heappop, heappush

GenericType = typing.TypeVar("GenericType")


class PriorityDict(typing.Generic[GenericType], typing.Dict[GenericType, float]):
    """Dictionary that can be used as a priority queue.

    Keys of the dictionary are items to be put into the queue, and values
    are their respective priorities. All dictionary methods work as expected.
    The advantage over a standard heapq-based priority queue is that priorities
    of items can be efficiently updated (amortized O(1)) using code as
    'thedict[item] = new_priority.'

    Note that this is a modified version of
    https://gist.github.com/matteodellamico/4451520 where sorted_iter() has
    been replaced with the destructive sorted iterator __iter__ from
    https://gist.github.com/anonymous/4435950
    """

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._rebuild_heap()

    def __setitem__(self, key: GenericType, val: float) -> None:
        super().__setitem__(key, val)

        if len(self._heap) < 2 * len(self):
            heappush(self._heap, (val, key))
        else:
            self._rebuild_heap()

    def __iter__(self) -> typing.Iterator[GenericType]:
        def item_generator() -> typing.Iterator[GenericType]:
            while len(self) > 0:
                x = self.smallest()
                yield x
                del self[x]

        return item_generator()

    def smallest(self) -> GenericType:
        heap = self._heap
        v, k = heap[0]
        while k not in self or self[k] != v:
            heappop(heap)
            v, k = heap[0]
        return k

    def pop_smallest(self) -> GenericType:
        heap = self._heap
        v, k = heappop(heap)
        while k not in self or self[k] != v:
            v, k = heappop(heap)
        del self[k]
        return k

    def update(self, *args, **kwargs):
        super().update(*args, **kwargs)
        self._rebuild_heap()

    def _rebuild_heap(self) -> None:
        self._heap = [(v, k) for k, v in iter(self.items())]
        heapify(self._heap)

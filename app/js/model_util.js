.pragma library

/*
 *  Searches a model for the given item by type and index
 *
 *  If the item is found later in the list, move it to the starting index
 */
function findItem(item_type, item_index, startIndex, model, params) {
    if (typeof(params) === 'undefined') {
        params = {}
    }

    var searchIndex = startIndex
    while (searchIndex < model.count)
    {
        var item = model.get(searchIndex)
        if (item.type == item_type &&
            item.itemIndex == item_index)
        {
            if (searchIndex != startIndex) {
                model.move(searchIndex, startIndex, 1)
            }
            return
        }
        searchIndex++;
    }

    params['type'] = item_type
    params['itemIndex'] = item_index
    model.insert(startIndex, params)
}

/*
 *  Erases remaining items and sets the 'last' property of a model
 */
function pop(model, i) {
    while (i < model.count) {
        model.remove(i++)
    }
    for (var i=0; i < model.count; ++i) {
        model.setProperty(i, 'last', i == model.count - 1)
    }
}
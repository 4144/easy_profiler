
function lower_bound(array, value)
{
    var len = array.length * 1 // to make sure that javascript copy variable array.length
    var i = len >> 1
    var left = 0

    while (len > 0)
    {
        len >>= 1

        if (array[i].rect.x < value)
        {
            left = i + 1
        }

        i = left + (len >> 1)
    }

    return i
}


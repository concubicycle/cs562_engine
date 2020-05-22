# System Bit Ranges and component bits. 


## Transform 

Bit shift range: **0-4**

```c++
	enum class component_bits : component_shift
	{
		transform = 0
	};
```

## Rendering
Bit shift range: **5-16**

```c++
	enum class component_bits : component_shift
	{
		camera                 = 1,
		light_directional      = 2,
		light_point            = 3,
		renderable_mesh_static = 4
	};
```

## Audio
Bit shift range: **17-24**

```c++
    enum class component_bits : component_shift
    {
        audio_emitter = 17,
        audio_listener = 18
    };
```

## Physics
Bit shift range: **25-31**
```c++
    enum class component_bits : component_shift
    {
        aabb_collider = 25,
        sphere_collider = 26,
        rigidbody = 27,
    };
```
## Game
Bit shift range: **32-64**

```c++
    enum class component_bits : component_shift
    {
        game_piece = 32,
		board = 33,
		board_square = 34
    };
```


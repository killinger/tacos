#pragma once
extern class console_system*		ConsoleSystem;
extern class render_system*			RenderSystem;
extern class logging_system*		LoggingSystem;
extern class input_handler*			InputHandler;
extern class gamestate_buffer*		GameStateBuffer;
extern class overlay_manager*		OverlayManager;
extern class debug_draw_manager*	DebugDrawManager;
extern class scene_graph*			SceneGraph;
#ifdef _PROFILE
extern class profile_system*		ProfileSystem;
#endif
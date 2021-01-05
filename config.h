/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 3;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 20;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 20;       /* vert outer gap between windows and screen edge */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=12", "JoyPixels:pixelsize=12:antialias=true:autohint=true"  };
static char dmenufont[]             = "monospace:size=12";
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#770000";
static char selbgcolor[]            = "#005577";
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};

//TODO: the terminal in scratchpads cmd won't get autosub, please change accordingly to $TERMINAL
typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = {"st", "-n", "spterm", "-g", "120x34", NULL };
const char *spcmd2[] = {"st", "-n", "spcalc", "-f", "monospace:size=16", "-g", "50x20", "-e", "bc", "-lq", NULL };
const char *spcmd3[] = {"sh", "actwinst" }; //see actwinst from examples
// TODO: if you want to add more scratchpads, PLEASE ADD THEM IN THE RULES AS WELL! 2020-Nov03 4.02AM
static Sp scratchpads[] = {
	/* name          cmd  */
	{"spterm",      spcmd1},
	{"spcalc",      spcmd2},
	{"spactw",   	spcmd3},
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	*/
	/* class    instance      title       	 tags mask    isfloating   isterminal  noswallow  monitor */
	{ "Gimp",     NULL,       NULL,       	    1 << 8,       0,           0,         0,        -1 },
	{ "St",       NULL,       NULL,       	    0,            0,           1,         0,        -1 },
	{ NULL,       NULL,       "Event Tester",   	0,            0,           0,         1,        -1 },
	{ NULL,       NULL,       "Emulator",		0,            1,           0,         0,        -1 }, //for android studio AVD
	{ NULL,      "spterm",    NULL,       	    SPTAG(0),     1,           1,         0,        -1 },
	{ NULL,      "spcalc",    NULL,       	    SPTAG(1),     1,           1,         0,        -1 },
	{ NULL,      "spactw",    NULL,       	    SPTAG(2),     1,           1,         0,        -1 },
};

/* layout(s) */
static const float mfact     = 0.50; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"
static const Layout layouts[] = {
	/* symbol     arrange function */
 	{ "[]=",	tile },			/* Default: Master on left, slaves on right */
	{ "TTT",	bstack },		/* Master on top, slaves on bottom */

	{ "[@]",	spiral },		/* Fibonacci spiral */
	{ "[\\]",	dwindle },		/* Decreasing in size right and leftward */

	{ "H[]",	deck },			/* Master on left, slaves in monocle-like mode on right */
 	{ "[M]",	monocle },		/* All windows on top of eachother */

	{ "|M|",	centeredmaster },		/* Master in middle, slaves on sides */
	{ ">M>",	centeredfloatingmaster },	/* Same but master floats */

	{ "><>",	NULL },			/* no layout function means floating behavior */
	{ NULL,		NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define STACKKEYS(MOD,ACTION) \
	{ MOD,	XK_j,	ACTION##stack,	{.i = INC(+1) } }, \
	{ MOD,	XK_k,	ACTION##stack,	{.i = INC(-1) } }, \
	{ MOD,  XK_v,   ACTION##stack,  {.i = 0 } }, \
	/* { MOD, XK_grave, ACTION##stack, {.i = PREVSEL } }, \ */
	/* { MOD, XK_a,     ACTION##stack, {.i = 1 } }, \ */
	/* { MOD, XK_z,     ACTION##stack, {.i = 2 } }, \ */
	/* { MOD, XK_x,     ACTION##stack, {.i = -1 } }, */

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };

#include <X11/XF86keysym.h>
#include "shiftview.c"
static Key keys[] = {
	/* modifier                     key        function        argument */
	STACKKEYS(MODKEY,                          focus)
	STACKKEYS(MODKEY|ShiftMask,                push)
	//{ MODKEY|ShiftMask,		XK_Escape,	spawn,	SHCMD("") },
	//{ MODKEY,			XK_grave,	spawn,	SHCMD("dmenuunicode") },
	//{ MODKEY|ShiftMask,		XK_grave,	togglescratch,	SHCMD("") },
	TAGKEYS(			XK_1,		0)
	TAGKEYS(			XK_2,		1)
	TAGKEYS(			XK_3,		2)
	TAGKEYS(			XK_4,		3)
	TAGKEYS(			XK_5,		4)
	TAGKEYS(			XK_6,		5)
	TAGKEYS(			XK_7,		6)
	TAGKEYS(			XK_8,		7)
	TAGKEYS(			XK_9,		8)
	{ MODKEY,			XK_0,		view,		{.ui = ~0 } }, //view all tags
	{ MODKEY|ShiftMask,		XK_0,		tag,		{.ui = ~0 } },
	//confirmed
	//------------------------------------------------------------------------------------------------------------------------------------------------------
	//NORMAL OPERATION (Nav, Control)
	//------------------------------------------------------------------------------------------------------------------------------------------------------
	/* J and K are automatically bound above in STACKEYS */
	/* V is automatically bound above in STACKKEYS */
	{ MODKEY,			XK_s,			togglesticky,	{0} },
	//{ MODKEY|ShiftMask,		XK_s,			spawn,		SHCMD("") },
	{ MODKEY,			XK_bracketleft,		shiftview,	{ .i = -1 } },
	{ MODKEY|ShiftMask,		XK_bracketleft,		shifttag,	{ .i = -1 } },
	{ MODKEY,			XK_bracketright,	shiftview,	{ .i = 1 } },
	{ MODKEY|ShiftMask,		XK_bracketright,	shifttag,	{ .i = 1 } },
	//{ MODKEY,			XK_Escape,		spawn,		SHCMD("") },
	//{ MODKEY|ShiftMask,		XK_Escape,		spawn,		SHCMD("") }, //too difficult to press
	{ MODKEY,			XK_BackSpace,		spawn,		SHCMD("prompt 'logout?' 'killall xinit'") }, //logout
	{ MODKEY|ShiftMask,		XK_BackSpace,		spawn,		SHCMD("prompt 'reboot?' 'reboot'") }, //reboot
	{ MODKEY,			XK_q,			killclient,	{0} }, //quit the window
	//{ MODKEY|ShiftMask,		XK_q,			quit,		{0} },
	{ MODKEY,			XK_b,			zoom,		{0} }, //bring the selected window as main
	{ MODKEY|ShiftMask,		XK_b,			togglebar,	{0} }, //toggle bar
	{ MODKEY,			XK_comma,		setmfact,	{.f = -0.05} }, //decrease master window size
	{ MODKEY|ShiftMask,		XK_comma,		incrgaps,	{.i = -3} }, //decrease gaps
	{ MODKEY,			XK_period,		setmfact,	{.f = +0.05} }, //increase master window size
	{ MODKEY|ShiftMask,		XK_period,		incrgaps,	{.i = +3} }, //increase gaps
	{ MODKEY,			XK_slash,		setmfact,	{.f = mfact+1.0} }, //restore default window size
	{ MODKEY|ShiftMask,		XK_slash,		defaultgaps,	{0} }, //restore default gaps
	{ MODKEY,			XK_backslash,		view,		{0} }, //switch to previous tag
	{ MODKEY|ShiftMask,		XK_backslash,		togglegaps,	{0} }, //toggle gaps
	{ MODKEY,			XK_Tab,			view,		{0} }, //switch to previous tag
	{ MODKEY,			XK_h,	focusmon,	{.i = -1 } }, //go to left mon
	{ MODKEY|ShiftMask,             XK_h,	tagmon,         {.i = +1 } }, //move window to left mon
	{ MODKEY,			XK_l,	focusmon,	{.i = +1 } }, //go to right mon
	{ MODKEY|ShiftMask,             XK_l,	tagmon,         {.i = -1 } }, //move window to right mon
	{ MODKEY|ControlMask,		XK_l,	spawn,		SHCMD("notify-send \"Locking out system\" \"Please stop working now.\"; mpc pause; screenlock;")}, //screen lock
	{ MODKEY|ShiftMask|ControlMask,	XK_l,	spawn,		SHCMD("prompt 'shutdown?' 'shutdown now'") }, //shutdown
	//------------------------------------------------------------------------------------------------------------------------------------------------------
	//PROGRAM SHORT KEYS
	//------------------------------------------------------------------------------------------------------------------------------------------------------
	{ MODKEY,			XK_a,		togglescratch,	{.ui = 1} }, //calculator
	{ MODKEY|ShiftMask,		XK_a,		spawn,		SHCMD("pamixer -t; kill -44 $(pidof dwmblocks)") }, //mute
	{ MODKEY,			XK_d,		spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,		XK_d,		spawn,          SHCMD("dmenumount") }, //mount menu
	{ MODKEY,			XK_c,		spawn,          SHCMD("dmenuunicode") }, //unicode menu
	{ MODKEY|ShiftMask,		XK_c,		spawn,          SHCMD("dmenuumount") }, //unmount menu
	//{ MODKEY,			XK_z,		spawn,          SHCMD("") },
	{ MODKEY|ShiftMask,		XK_z,		spawn,          SHCMD("mailsync") }, //sync mail
	{ MODKEY,			XK_w,		spawn,		SHCMD("$BROWSER") },
	{ MODKEY|ShiftMask,		XK_w,		spawn,		SHCMD("$BROWSER") },
	{ MODKEY,			XK_x,		spawn,		SHCMD("displayselect") },
	{ MODKEY|ShiftMask,		XK_x,		spawn,		SHCMD("xrdb $HOME/.config/Xresources") }, //refresh xresources
	{ MODKEY,			XK_e,		spawn,		SHCMD("$TERMINAL -e neomutt ; pkill -RTMIN+12 dwmblocks; rmdir ~/.abook") },
	{ MODKEY|ShiftMask,		XK_e,		spawn,		SHCMD("$TERMINAL -e abook -C ~/.config/abook/abookrc --datafile ~/.config/abook/addressbook") },
	{ MODKEY,			XK_r,		spawn,		SHCMD("$TERMINAL -e $FILE") },
	{ MODKEY|ShiftMask,		XK_r,		spawn,		SHCMD("$TERMINAL -e htop") },
	{ MODKEY,			XK_p,		spawn,		SHCMD("mpc toggle; pkill -RTMIN+10 dwmblocks") },
	{ MODKEY|ShiftMask,		XK_p,		spawn,		SHCMD("mpc pause; pkill -RTMIN+10 dwmblocks") },
	{ MODKEY,			XK_m,		spawn,		SHCMD("$TERMINAL -e ncmpcpp") }, //ncmpcpp
	{ MODKEY|ShiftMask,		XK_m,		spawn,		SHCMD("$TERMINAL -e pulsemixer; pkill -RTMIN+10 dwmblocks") }, //audio check
	{ MODKEY,			XK_n,		spawn,		SHCMD("sudo -A systemctl restart wpa_supplicant-nl80211@wifi0") }, //restart wpa_supplicant
	{ MODKEY|ShiftMask,		XK_n,		spawn,		SHCMD("sudo -A systemctl stop wpa_supplicant-nl80211@wifi0") }, //stop wpa_supplicant on wifi0
	{ MODKEY,			XK_o,		spawn,		SHCMD("torwrap") }, //launch the transmission cli app
	{ MODKEY|ShiftMask,		XK_o,		spawn,		SHCMD("td-toggle") }, //start the torrent daemon
	{ MODKEY,			XK_apostrophe,	togglescratch,	{.ui = 0} }, //spawn floating terminal
	{ MODKEY|ShiftMask,		XK_apostrophe,	togglescratch,	{.ui = 2} }, //spawn floating terminal as samedir
	{ MODKEY,			XK_Return,	spawn,		SHCMD("$TERMINAL") }, //spawn new terminal
	{ MODKEY|ShiftMask,		XK_Return,	spawn,		SHCMD("samedir") }, //spawn a terminal on the same directory as active window
	{ MODKEY,			XK_minus,	spawn,		SHCMD("pamixer --allow-boost -d 5; kill -44 $(pidof dwmblocks)") }, //decrease volume
	{ MODKEY|ShiftMask,		XK_minus,	spawn,		SHCMD("pamixer --allow-boost -d 15; kill -44 $(pidof dwmblocks)") }, //decrease volume more
	{ MODKEY,			XK_equal,	spawn,		SHCMD("pamixer --allow-boost -i 5; kill -44 $(pidof dwmblocks)") }, //increase volume
	{ MODKEY|ShiftMask,		XK_equal,	spawn,		SHCMD("pamixer --allow-boost -i 15; kill -44 $(pidof dwmblocks)") }, //increase volume more
	{ 0,				XK_Print,	spawn,		SHCMD("maim pic-full-$(date '+%y%m%d-%H%M-%S').png") }, //screenshot entire screen
	{ ShiftMask,			XK_Print,	spawn,		SHCMD("maimpick") }, //select from list of screenshot options
	{ MODKEY,			XK_Insert,	spawn,		SHCMD("notify-send \"📋 Clipboard contents:\" \"$(xclip -o -selection clipboard)\"") },
	//------------------------------------------------------------------------------------------------------------------------------------------------------
	//LAYOUTS ADJUSTMENTS
	//------------------------------------------------------------------------------------------------------------------------------------------------------
	{ MODKEY,			XK_t,		setlayout,	{.v = &layouts[0]} }, /* tile */
	{ MODKEY|ShiftMask,		XK_t,		setlayout,	{.v = &layouts[1]} }, /* bstack */
	{ MODKEY,			XK_y,		setlayout,	{.v = &layouts[2]} }, /* spiral */
	{ MODKEY|ShiftMask,		XK_y,		setlayout,	{.v = &layouts[3]} }, /* dwindle */
	{ MODKEY,			XK_u,		setlayout,	{.v = &layouts[4]} }, /* deck */
	{ MODKEY|ShiftMask,		XK_u,		setlayout,	{.v = &layouts[5]} }, /* monocle */
	{ MODKEY,			XK_i,		setlayout,	{.v = &layouts[6]} }, /* centeredmaster */
	{ MODKEY|ShiftMask,		XK_i,		setlayout,	{.v = &layouts[7]} }, /* centeredfloatingmaster */
	{ MODKEY,			XK_f,		togglefullscr,	{0} },
	{ MODKEY|ShiftMask,		XK_f,		setlayout,	{.v = &layouts[8]} }, /* toggle floating */
	{ MODKEY,			XK_space,	zoom,		{0} }, //bring the selected window as main
	{ MODKEY|ShiftMask,		XK_space,	togglefloating,	{0} },
	//------------------------------------------------------------------------------------------------------------------------------------------------------
	//SPECIAL KEYS
	//------------------------------------------------------------------------------------------------------------------------------------------------------
	{ 0, XF86XK_AudioMute,		spawn,		SHCMD("pamixer -t; kill -44 $(pidof dwmblocks)") },
	{ 0, XF86XK_AudioRaiseVolume,	spawn,		SHCMD("pamixer --allow-boost -i 3; kill -44 $(pidof dwmblocks)") },
	{ 0, XF86XK_AudioLowerVolume,	spawn,		SHCMD("pamixer --allow-boost -d 3; kill -44 $(pidof dwmblocks)") },
	{ 0, XF86XK_AudioPrev,		spawn,		SHCMD("mpc prev") },
	{ 0, XF86XK_AudioNext,		spawn,		SHCMD("mpc next") },
	{ 0, XF86XK_AudioPause,		spawn,		SHCMD("mpc pause") },
	{ 0, XF86XK_AudioPlay,		spawn,		SHCMD("mpc play") },
	{ 0, XF86XK_AudioStop,		spawn,		SHCMD("mpc stop") },
	{ 0, XF86XK_AudioRewind,	spawn,		SHCMD("mpc seek -10") },
	{ 0, XF86XK_AudioForward,	spawn,		SHCMD("mpc seek +10") },
	{ 0, XF86XK_AudioMedia,		spawn,		SHCMD("$TERMINAL -e ncmpcpp") },
	{ 0, XF86XK_PowerOff,		spawn,		SHCMD("shutdown now") },
	{ 0, XF86XK_Calculator,		spawn,		SHCMD("$TERMINAL -e bc -l") },
	{ 0, XF86XK_Sleep,		spawn,		SHCMD("prompt 'hibernate?' 'sudo -A systemctl suspend") },
	{ 0, XF86XK_WWW,		spawn,		SHCMD("$BROWSER") },
	{ 0, XF86XK_DOS,		spawn,		SHCMD("$TERMINAL") },
	{ 0, XF86XK_ScreenSaver,	spawn,		SHCMD("xset dpms force off;") },
	{ 0, XF86XK_TaskPane,		spawn,		SHCMD("$TERMINAL -e htop") },
	{ 0, XF86XK_Mail,		spawn,		SHCMD("$TERMINAL -e neomutt ; pkill -RTMIN+12 dwmblocks") },
	{ 0, XF86XK_MyComputer,		spawn,		SHCMD("$TERMINAL -e $FILE /") },
	/* { 0, XF86XK_Battery,		spawn,		SHCMD("") }, */
	{ 0, XF86XK_Launch1,		spawn,		SHCMD("xset dpms force off") },
	{ 0, XF86XK_TouchpadToggle,	spawn,		SHCMD("(synclient | grep 'TouchpadOff.*1' && synclient TouchpadOff=0) || synclient TouchpadOff=1") },
	{ 0, XF86XK_TouchpadOff,	spawn,		SHCMD("synclient TouchpadOff=1") },
	{ 0, XF86XK_TouchpadOn,		spawn,		SHCMD("synclient TouchpadOff=0") },
	{ 0, XF86XK_MonBrightnessUp,	spawn,		SHCMD("light -A 5") },
	{ 0, XF86XK_MonBrightnessDown,	spawn,		SHCMD("light -U 5") },
	//------------------------------------------------------------------------------------------------------------------------------------------------------
	//FUNCTION KEYS
	//------------------------------------------------------------------------------------------------------------------------------------------------------
	//{ MODKEY,			XK_F1,		spawn,		SHCMD("groff -mom /usr/local/share/dwm/larbs.mom -Tpdf | zathura -") },
	//{ MODKEY,			XK_F1,		spawn,		SHCMD("mailsync") },
	{ MODKEY,			XK_F1,		spawn,		SHCMD("$TERMINAL -e less $HOME/github/impc/dwm/config.h") },
	{ MODKEY,			XK_F2,		spawn,		SHCMD("killall screenkey || screenkey &") },
	{ MODKEY,			XK_F3,		spawn,		SHCMD("dmenumount") },
	{ MODKEY,			XK_F4,		spawn,		SHCMD("dmenuumount") },
	//{ MODKEY,			XK_F5,		xrdb,		{.v = NULL } },
	{ MODKEY,			XK_F5,		quit,		{0} }, // quits/refreshes dwm
	{ MODKEY,			XK_F6,		spawn,		SHCMD("torwrap") },
	{ MODKEY,			XK_F7,		spawn,		SHCMD("td-toggle") },
	{ MODKEY,			XK_F8,		spawn,		SHCMD("mailsync") },
	//{ MODKEY,			XK_F9,		spawn,		SHCMD("dmenumount") },
	//{ MODKEY,			XK_F10,		spawn,		SHCMD("dmenuumount") },
	//{ MODKEY,			XK_F11,		spawn,		SHCMD("") },
	{ MODKEY,			XK_F12,		xrdb,		{.v = NULL } },
	//------------------------------------------------------------------------------------------------------------------------------------------------------
	//staging area (testing its ergonomic)
	//------------------------------------------------------------------------------------------------------------------------------------------------------
	{ MODKEY,			XK_Page_Up,	shiftview,	{ .i = -1 } },
	{ MODKEY|ShiftMask,		XK_Page_Up,	shifttag,	{ .i = -1 } },
	{ MODKEY,			XK_Page_Down,	shiftview,	{ .i = +1 } },
	{ MODKEY|ShiftMask,		XK_Page_Down,	shifttag,	{ .i = +1 } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigdwmblocks,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigdwmblocks,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigdwmblocks,   {.i = 3} },
	{ ClkStatusText,        0,              Button4,        sigdwmblocks,   {.i = 4} },
	{ ClkStatusText,        0,              Button5,        sigdwmblocks,   {.i = 5} },
	{ ClkStatusText,        ShiftMask,      Button1,        sigdwmblocks,   {.i = 6} },
	{ ClkStatusText,        ShiftMask,      Button3,        spawn,          SHCMD("st -e nvim ~/.local/src/dwmblocks/config.h") },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        defaultgaps,	{0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkClientWin,		MODKEY,		Button4,	incrgaps,	{.i = +1} },
	{ ClkClientWin,		MODKEY,		Button5,	incrgaps,	{.i = -1} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkTagBar,		0,		Button4,	shiftview,	{.i = -1} },
	{ ClkTagBar,		0,		Button5,	shiftview,	{.i = 1} },
	{ ClkRootWin,		0,		Button2,	togglebar,	{0} },
};

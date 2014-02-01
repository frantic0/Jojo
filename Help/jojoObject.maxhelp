{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 5,
			"minor" : 1,
			"revision" : 9
		}
,
		"rect" : [ 79.0, 132.0, 423.0, 375.0 ],
		"bglocked" : 0,
		"defrect" : [ 79.0, 132.0, 423.0, 375.0 ],
		"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
		"openinpresentation" : 0,
		"default_fontsize" : 14.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 0,
		"gridsize" : [ 20.0, 20.0 ],
		"gridsnaponopen" : 0,
		"toolbarvisible" : 1,
		"boxanimatetime" : 200,
		"imprint" : 0,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"boxes" : [ 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "spawn",
					"id" : "obj-5",
					"fontname" : "Arial",
					"patching_rect" : [ 180.0, 80.0, 51.0, 21.0 ],
					"numinlets" : 2,
					"fontsize" : 14.0,
					"numoutlets" : 1,
					"outlettype" : [ "" ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "clone",
					"id" : "obj-4",
					"fontname" : "Arial",
					"patching_rect" : [ 260.0, 80.0, 44.0, 21.0 ],
					"numinlets" : 2,
					"fontsize" : 14.0,
					"numoutlets" : 1,
					"outlettype" : [ "" ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"id" : "obj-3",
					"patching_rect" : [ 120.0, 80.0, 20.0, 20.0 ],
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "jojoObject",
					"id" : "obj-1",
					"fontname" : "Arial",
					"patching_rect" : [ 120.0, 220.0, 73.0, 23.0 ],
					"numinlets" : 1,
					"fontsize" : 14.0,
					"numoutlets" : 0
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"source" : [ "obj-5", 0 ],
					"destination" : [ "obj-1", 0 ],
					"hidden" : 0,
					"midpoints" : [ 189.5, 142.0, 129.5, 142.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-3", 0 ],
					"destination" : [ "obj-1", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-4", 0 ],
					"destination" : [ "obj-1", 0 ],
					"hidden" : 0,
					"midpoints" : [ 269.5, 142.0, 129.5, 142.0 ]
				}

			}
 ]
	}

}

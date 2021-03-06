#import <Cocoa/Cocoa.h>

#define CAML_NAME_SPACE

#import <caml/mlvalues.h>
#import <caml/memory.h>
#import <caml/callback.h>
#import <caml/alloc.h>
#import <caml/bigarray.h>
#import <caml/custom.h>
#import <caml/fail.h>
#import <caml/threads.h>

#define Val_NSApplication(v) ((value)(v))
#define NSApplication_val(v) ((__bridge NSApplication *)(value)(v))

#define Val_NSWindow(v) ((value)(v))
#define NSWindow_val(v) ((__bridge NSWindow *)(value)(v))

@interface View : NSView
@end

@implementation View

- (BOOL)isFlipped
{
  return YES;
}

@end

typedef void (^ActionBlock)();

@interface Button : NSButton

@property(nonatomic, copy) ActionBlock _actionBlock;

- (void)onClick:(ActionBlock)action;
@end

@implementation Button

- (void)onClick:(ActionBlock)action
{
  self._actionBlock = action;
  [self setTarget:self];
  [self setAction:@selector(callActionBlock:)];
}

- (void)callActionBlock:(id)__unused sender
{
  self._actionBlock();
}
@end

#define Val_View(v) ((value)(v))
#define View_val(v) ((__bridge View *)(value)(v))

#define Val_Button(v) ((value)(v))
#define Button_val(v) ((__bridge Button *)(value)(v))

static NSMutableDictionary *ml_Views;
static NSMutableArray *ml_Views_all;

CAMLprim value ml_NSLog(value str)
{
  CAMLparam1(str);
  NSLog(@"%s", String_val(str));
  CAMLreturn(Val_unit);
}

@interface MLApplicationDelegate : NSObject <NSApplicationDelegate>

@end

enum
{
  ApplicationWillFinishLaunching,
  ApplicationDidFinishLaunching
};

@implementation MLApplicationDelegate
{
  value applicationId;
}

- (instancetype)initWithId:(value)appId
{
  if (self = [super init])
  {
    applicationId = appId;
  }
  ml_Views = [NSMutableDictionary new];
  ml_Views_all = [NSMutableArray new];
  return self;
}

- (void)applicationWillTerminate:(NSNotification *)__unused not
{
  // applicationWillTerminate
}

- (void)applicationWillFinishLaunching:(NSNotification *)__unused not
{
  static value *closure_f = NULL;

  if (closure_f == NULL)
  {
    closure_f = caml_named_value("NSApp.delegate");
  }

  if (closure_f != NULL)
  {
    caml_callback2(*closure_f, applicationId, Val_int(ApplicationWillFinishLaunching));
  }
}

- (void)applicationDidFinishLaunching:(NSNotification *)__unused not
{
  static value *closure_f = NULL;

  if (closure_f == NULL)
  {
    closure_f = caml_named_value("NSApp.delegate");
  }

  if (closure_f != NULL)
  {
    caml_callback2(*closure_f, applicationId, Val_int(ApplicationDidFinishLaunching));
  }
}

@end

@interface MLWindowDelegate : NSObject <NSWindowDelegate>

@end

enum
{
  WindowDidResize
};

@implementation MLWindowDelegate
{
  value windowId;
}

- (instancetype)initWithId:(value)winId
{
  if (self = [super init])
  {
    windowId = winId;
  }
  return self;
}

- (void)windowDidResize:(NSNotification *)__unused aNotification
{
  static value *closure_f = NULL;

  if (closure_f == NULL)
  {
    closure_f = caml_named_value("NSWindow.delegate");
  }

  caml_callback2(*closure_f, windowId, Val_int(WindowDidResize));
}

@end

CAMLprim value ml_NSWindow_makeWithContentRect(value winId, value x_v, value y_v, value w_v, value h_v)
{
  CAMLparam5(winId, x_v, y_v, w_v, h_v);
  CGFloat x = Double_val(x_v);
  CGFloat y = Double_val(y_v);
  CGFloat w = Double_val(w_v);
  CGFloat h = Double_val(h_v);

  NSRect contentRect = NSMakeRect(x, y, w, h);

  NSUInteger styleMask =
      NSWindowStyleMaskTitled |
      NSWindowStyleMaskClosable |
      NSWindowStyleMaskMiniaturizable |
      NSWindowStyleMaskResizable;

  NSWindow *win = [[NSWindow alloc] initWithContentRect:contentRect
                                              styleMask:styleMask
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
  [win setDelegate:[[MLWindowDelegate alloc] initWithId:winId]];

  CAMLreturn(Val_NSWindow(win));
}

CAMLprim value ml_NSWindow_center(value win_v)
{
  CAMLparam1(win_v);
  NSWindow *win = NSWindow_val(win_v);
  [win center];
  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSWindow_makeKeyAndOrderFront(value win_v)
{
  CAMLparam1(win_v);
  NSWindow *win = NSWindow_val(win_v);
  [win makeKeyAndOrderFront:nil];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSWindow_isVisible(value win_v)
{
  CAMLparam1(win_v);
  NSWindow *win = NSWindow_val(win_v);
  BOOL b = [win isVisible];

  CAMLreturn(Val_bool(b));
}

CAMLprim value ml_NSWindow_contentView(value win_v)
{
  CAMLparam1(win_v);
  NSWindow *win = NSWindow_val(win_v);

  CAMLreturn(Val_View(win.contentView));
}

CAMLprim value ml_NSWindow_title(value win_v)
{
  CAMLparam1(win_v);
  CAMLlocal1(str_v);

  NSWindow *win = NSWindow_val(win_v);
  NSString *str = [win title];
  const char *bytes = [str UTF8String];
  int len = strlen(bytes);
  str_v = caml_alloc_string(len);
  memcpy(String_val(str_v), bytes, len);

  CAMLreturn(str_v);
}

CAMLprim value ml_NSWindow_setContentView(value win_v, value view_v)
{
  CAMLparam1(win_v);
  NSWindow *win = NSWindow_val(win_v);
  View *view = View_val(view_v);

  [view setWantsLayer:YES];

  [win setContentView:view];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSWindow_setTitle(value win_v, value str_v)
{
  CAMLparam2(win_v, str_v);
  NSWindow *win = NSWindow_val(win_v);
  NSString *str = [NSString stringWithUTF8String:String_val(str_v)];
  [win setTitle:str];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSApplication_NSApp(value appId)
{
  CAMLparam1(appId);
  NSApplication *app = [NSApplication sharedApplication];
  [app setActivationPolicy:NSApplicationActivationPolicyRegular];
  [app activateIgnoringOtherApps:YES];
  [app setDelegate:[[MLApplicationDelegate alloc] initWithId:appId]];

  CAMLreturn(Val_NSApplication(app));
}

CAMLprim value ml_NSApplication_run(value appId)
{
  CAMLparam1(appId);

  @autoreleasepool
  {
    NSApplication *app = [NSApplication sharedApplication];
    [app run];
  }

  CAMLreturn(Val_unit);
}

// View
CAMLprim value ml_NSView_make()
{
  CAMLparam0();

  View *view = nil;
  view = [View new];

  [ml_Views_all addObject:view];

  CAMLreturn(Val_View(view));
}

CAMLprim value ml_NSView_memoize(value id_v, value view_v)
{
  CAMLparam2(id_v, view_v);
  View *view = View_val(view_v);

  [ml_Views setObject:view forKey:@(Int_val(id_v))];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSView_free(value id_v)
{
  CAMLparam1(id_v);

  [ml_Views removeObjectForKey:@(Int_val(id_v))];
  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSView_addSubview(value view_v, value child_v)
{
  CAMLparam2(view_v, child_v);
  View *view = View_val(view_v);
  View *child = View_val(child_v);

  [view addSubview:child];

  CAMLreturn(Val_View(view));
}

CAMLprim value ml_NSView_removeSubview(value child_v)
{
  CAMLparam1(child_v);
  View *child = View_val(child_v);

  [child removeFromSuperview];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSView_setFrame(value view_v, value x_v, value y_v, value w_v, value h_v)
{
  CAMLparam5(view_v, x_v, y_v, w_v, h_v);

  View *view = View_val(view_v);

  CGFloat x = Double_val(x_v);
  CGFloat y = Double_val(y_v);
  CGFloat w = Double_val(w_v);
  CGFloat h = Double_val(h_v);

  NSRect rect = NSMakeRect(x, y, w, h);
  [view setFrame:rect];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSView_setBorderWidth(value view_v, value width_v)
{
  CAMLparam2(view_v, width_v);

  View *view = View_val(view_v);

  [view.layer setBorderWidth:Int_val(width_v)];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSView_setBorderColor(value view_v, value red_v, value green_v, value blue_v, value alpha_v)
{
  CAMLparam5(view_v, red_v, green_v, blue_v, alpha_v);

  View *view = View_val(view_v);

  CGFloat red = Double_val(red_v) / 255;
  CGFloat green = Double_val(green_v) / 255;
  CGFloat blue = Double_val(blue_v) / 255;
  CGFloat alpha = Double_val(alpha_v);

  [view setWantsLayer:YES];
  [view.layer setBorderColor:[[NSColor colorWithRed:red green:green blue:blue alpha:alpha] CGColor]];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSView_setBackgroundColor(value view_v, value red_v, value green_v, value blue_v, value alpha_v)
{
  CAMLparam5(view_v, red_v, blue_v, green_v, alpha_v);

  View *view = View_val(view_v);

  CGFloat red = Double_val(red_v) / 255;
  CGFloat blue = Double_val(blue_v) / 255;
  CGFloat green = Double_val(green_v) / 255;
  CGFloat alpha = Double_val(alpha_v);

  [view setWantsLayer:YES];
  [view.layer setBackgroundColor:[[NSColor colorWithRed:red green:green blue:blue alpha:alpha] CGColor]];

  CAMLreturn(Val_unit);
}

// NSButton
CAMLprim value ml_NSButton_make()
{
  CAMLparam0();

  Button *btn = nil;
  btn = [Button new];
  [ml_Views_all addObject:btn];

  CAMLreturn(Val_Button(btn));
}

CAMLprim value ml_NSButton_setFrame(value btn_v, value x_v, value y_v, value w_v, value h_v)
{
  CAMLparam5(btn_v, x_v, y_v, w_v, h_v);

  Button *btn = Button_val(btn_v);

  CGFloat x = Double_val(x_v);
  CGFloat y = Double_val(y_v);
  CGFloat w = Double_val(w_v);
  CGFloat h = Double_val(h_v);

  NSRect rect = NSMakeRect(x, y, w, h);
  [btn setFrame:rect];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSButton_setCallback(value btn_v, value callback_v)
{
  CAMLparam2(btn_v, callback_v);

  Button *btn = Button_val(btn_v);
  value callback = callback_v;

  caml_register_global_root(&callback);

  [btn onClick:^{
    caml_callback(callback, Val_unit);
  }];

  CAMLreturn(Val_Button(btn));
}

CAMLprim value ml_NSButton_setTitle(value btn_v, value str_v)
{
  CAMLparam2(btn_v, str_v);
  Button *btn = Button_val(btn_v);
  NSString *str = [NSString stringWithUTF8String:String_val(str_v)];
  [btn setTitle:str];

  CAMLreturn(Val_Button(btn));
}

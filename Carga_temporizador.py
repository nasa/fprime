export to My CVE201806
export to CI
export to union
export to OceaniaCloud
import datetime from watch.sec
import sthereos from arc('/Saturn.py') & arc('/mundi.py') 

import watch.sec.carga
 .start (count_down.time)-(count_watch.time)=watch.sec.now.online;
  Return, pass 'carga' ? true or false;
   Result =.     And import & add ciview to list (./farenheight.py)
    Close watch.sec.carga

#import "Kraken.h"
#import "KrakenPlugin.h"

static FlutterMethodChannel *methodChannel = nil;

@implementation KrakenPlugin

+ (FlutterMethodChannel *) getMethodChannel {
  return methodChannel;
}

+ (void)registerWithRegistrar:(NSObject<FlutterPluginRegistrar>*)registrar {
  NSObject<FlutterBinaryMessenger>* messager = [registrar messenger];
  FlutterMethodChannel* channel = [FlutterMethodChannel
      methodChannelWithName:@"kraken"
            binaryMessenger:messager];

  methodChannel = channel;

  KrakenPlugin* instance = [[KrakenPlugin alloc] initWithRegistrar: registrar];

  [registrar addMethodCallDelegate:instance channel:channel];
}

- (instancetype) initWithRegistrar: (NSObject<FlutterPluginRegistrar>*)registrar {
  self = [super init];
  self.registrar = registrar;
  return self;
}

- (void)handleMethodCall:(FlutterMethodCall*)call result:(FlutterResult)result {
  if ([@"getUrl" isEqualToString:call.method]) {
    Kraken* krakenInstance = [Kraken instanceByBinaryMessenger: [self.registrar messenger]];
    if (krakenInstance != nil) {
      result([krakenInstance getUrl]);
    } else {
      result(nil);
    }
  } else if ([@"invokeMethod" isEqualToString: call.method]) {
    Kraken* krakenInstance = [Kraken instanceByBinaryMessenger: [self.registrar messenger]];
    FlutterMethodCall* callWrap = [FlutterMethodCall methodCallWithMethodName: call.arguments[@"method"] arguments: call.arguments[@"args"]];
    [krakenInstance _handleMethodCall:callWrap result:result];
  } else if ([@"getTemporaryDirectory" isEqualToString: call.method]) {
    result([self getTemporaryDirectory]);
  } else {
    result(FlutterMethodNotImplemented);
  }
}

- (NSString*) getTemporaryDirectory {
  NSArray<NSString *>* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
  return [paths.firstObject stringByAppendingString: @"/Kraken"];
}

@end

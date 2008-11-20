//
//  OperationIntegrate.m
//  mathomatic
//
//  Created by Ben Gotow on 10/25/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "OperationUnfactor.h"
#import "mathomaticViewController.h"


@implementation OperationUnfactor

- (void)run
{
    MathCommand * c = [[MathCommand alloc] init];
    [c addInput: entry];
    [c addInput: @"unfactor fully"];
    [delegate performCommand: c];
    [c release];
    [self release];
}


@end

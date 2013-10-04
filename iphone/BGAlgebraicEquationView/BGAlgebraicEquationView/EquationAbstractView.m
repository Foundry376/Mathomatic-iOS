//
//  EquationAbstractView.m
//  mathomatic
//
//  Created by Ben Gotow on 11/14/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "EquationAbstractView.h"


@implementation EquationAbstractView

@synthesize alignmentMethod;

- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        self.opaque = NO;
        self.backgroundColor = [UIColor clearColor];
        self.alignmentMethod = kAlignmentMethodBottom;
    }
    return self;
}

- (void)finalizeEquationTree:(BOOL)clean
{
}

- (void)finalizeTextSize:(int)parentTextSize
{
}

- (BOOL)isTreeLegal
{
    return YES;
}

- (void)unlink
{
}

- (void)dealloc {
    [super dealloc];
}


@end

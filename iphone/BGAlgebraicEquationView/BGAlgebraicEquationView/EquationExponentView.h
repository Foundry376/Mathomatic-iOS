//
//  EquationExponentView.h
//  mathomatic
//
//  Created by Ben Gotow on 11/14/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "EquationTwoPartView.h"

@interface EquationExponentView : EquationTwoPartView {
}

- (id)initWithParent:(EquationContainerView*)p;
- (void)finalizeTextSize:(int)parentTextSize;
- (void)finalizeFrame;
- (NSString*)description;
- (void)dealloc;

@end
